/*
 * Copyright (c) 2014 David Wicks, sansumbrella.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "treent/Node3D.h"
#include "treent/GuiSystem.h"
#include "pockets/CollectionUtilities.hpp"
#include "cinder/gl/gl.h"

using namespace std;
using namespace cinder;
using namespace pockets;

namespace treent
{

Node3D::Node3D( const Entity &entity ):
  mEntity( entity )
{
  mTransform = mEntity.assign<Transform3D>();
}

Node3D::~Node3D()
{
  for( auto &child : mChildren )
  {
    child->mParent = nullptr;
  }
  mEntity.destroy();
}

void Node3D::appendChild( Node3DRef element )
{
  insertChildAt( element, mChildren.size() );
}

void Node3D::insertChildAt( Node3DRef child, size_t index )
{
  child->setParent( this );
  mChildren.insert( mChildren.begin() + index, child );
  childAdded( child );
}

void Node3D::setChildIndex( Node3DRef child, size_t index )
{
  vector_remove( &mChildren, child );
  index = math<int32_t>::min( index, mChildren.size() );
  mChildren.insert( mChildren.begin() + index, child );
}

void Node3D::removeChild( Node3DRef element )
{
  vector_remove( &mChildren, element );
  element->mParent = nullptr;
}

void Node3D::removeChild( Node3D *element )
{
  vector_erase_if( &mChildren, [element]( Node3DRef &n ){ return n.get() == element; } );
  element->mParent = nullptr;
}

void Node3D::clearChildren()
{
	for( auto &child : mChildren ) {
		child->mParent = nullptr;
	}
	mChildren.clear();
}

void Node3D::setParent( Node3D *parent )
{
  if( mParent && mParent != parent )
  { mParent->removeChild( this ); }
  mParent = parent;
}

bool Node3D::deepTouchesBegan( ci::app::TouchEvent &event )
{
  bool captured = false;
  auto gui_component = component<GuiComponent>();
  if( gui_component )
    captured = gui_component->touchesBegan( event, getWorldTransform() );

  for( Node3DRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
      break;

    captured = child->deepTouchesBegan( event );
  }
  return captured;
}

bool Node3D::deepTouchesMoved( ci::app::TouchEvent &event )
{
  bool captured = false;
  auto gui_component = component<GuiComponent>();
  if( gui_component )
    captured = gui_component->touchesMoved( event, getWorldTransform() );

  for( Node3DRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
      break;

    captured = child->deepTouchesMoved( event );
  }
  return captured;
}

bool Node3D::deepTouchesEnded( ci::app::TouchEvent &event )
{
  bool captured = false;
  auto gui_component = component<GuiComponent>();
  if( gui_component )
    captured = gui_component->touchesEnded( event, getWorldTransform() );

  for( Node3DRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
      break;

    captured = child->deepTouchesEnded( event );
  }
  return captured;
}

bool Node3D::deepMouseDown( ci::app::MouseEvent &event )
{
  bool captured = false;
  auto gui_component = component<GuiComponent>();
  if( gui_component )
    captured = gui_component->mouseDown( event, getWorldTransform() );

  for( Node3DRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
      break;

    captured = child->deepMouseDown( event );
  }
  return captured;
}

bool Node3D::deepMouseDrag( ci::app::MouseEvent &event )
{
  bool captured = false;
  auto gui_component = component<GuiComponent>();
  if( gui_component )
    captured = gui_component->mouseDrag( event, getWorldTransform() );

  for( Node3DRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
      break;

    captured = child->deepMouseDrag( event );
  }
  return captured;
}

bool Node3D::deepMouseUp( ci::app::MouseEvent &event )
{
  bool captured = false;
  auto gui_component = component<GuiComponent>();
  if( gui_component )
    captured = gui_component->mouseUp( event, getWorldTransform() );

  for( Node3DRef &child : mChildren )
  { // stop evaluation if event was captured by self or a child
    if( captured )
      break;

    captured = child->deepMouseUp( event );
  }
  return captured;
}

void Node3D::updateTree( const ci::MatrixAffine2f &matrix )
{
  mTransform->updateMatrix( matrix );
  for( Node3DRef &child : mChildren ) {
    child->updateTree( mTransform->matrix );
  }
}

void Node3D::deepCancelInteractions()
{
  cancelInteractions();
  for( Node3DRef &child : mChildren ) {
    child->deepCancelInteractions();
  }
}

//
//  MARK: - RootNode
//
void RootNode::connect( ci::app::WindowRef window )
{
  storeConnection( window->getSignalTouchesBegan().connect( [this]( app::TouchEvent &event )
                                                           {
                                                             deepTouchesBegan( event );
                                                           } ) );
  storeConnection( window->getSignalTouchesMoved().connect( [this]( app::TouchEvent &event )
                                                           {
                                                             deepTouchesMoved( event );
                                                           } ) );
  storeConnection( window->getSignalTouchesEnded().connect( [this]( app::TouchEvent &event )
                                                           {
                                                             deepTouchesEnded( event );
                                                           } ) );

  storeConnection( window->getSignalMouseDown().connect( [this]( app::MouseEvent &event )
                                                        {
                                                          deepMouseDown( event );
                                                        } ) );
  storeConnection( window->getSignalMouseDrag().connect( [this]( app::MouseEvent &event )
                                                        {
                                                          deepMouseDrag( event );
                                                        } ) );
  storeConnection( window->getSignalMouseUp().connect( [this]( app::MouseEvent &event )
                                                      {
                                                        deepMouseUp( event );
                                                      } ) );
}

} // treent::
