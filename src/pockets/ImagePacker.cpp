/*
 * Copyright (c) 2013 David Wicks
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

#include "ImagePacker.h"
using namespace ci;
using namespace std;
using namespace pockets;

ImagePacker::ImagePacker()
{}

ImagePacker::~ImagePacker()
{}

void ImagePacker::addImage( ci::Surface surface, const std::string &id, const ci::fs::path &file_path )
{
  mImages.push_back( ImageData( surface, id, file_path ) );
}

void ImagePacker::addFont(const ci::Font &font, const std::string &glyphs)
{
  
}

JsonTree ImagePacker::surfaceDescription()
{
  JsonTree description;
  JsonTree metaData = JsonTree::makeObject("meta");
  metaData.pushBack( JsonTree("created_by", "David Wicks' ImagePacker") );
  metaData.pushBack( JsonTree("width", mWidth ) );
  metaData.pushBack( JsonTree("height", mHeight ) );
  description.pushBack( metaData );

  JsonTree sprites = JsonTree::makeArray("sprites");
  for( ImageData &sprite : mImages )
  {
    sprites.pushBack( sprite.toJson() );
  }
  description.pushBack( sprites );

  return description;
}

Surface ImagePacker::packedSurface()
{
  Surface output( mWidth, mHeight, true, SurfaceChannelOrder::RGBA );
  for( ImageData &sprite : mImages )
  {
    output.copyFrom( sprite.getSurface(), sprite.getBounds(), sprite.getLoc() );
  }
  return output;
}

void ImagePacker::calculatePositions()
{
  auto rev_area_compare = []( const ImageData &lhs, const ImageData &rhs )
  {
    return lhs.getBounds().calcArea() > rhs.getBounds().calcArea();
  };
  auto rev_height_compare = []( const ImageData &lhs, const ImageData &rhs )
  {
    return lhs.getBounds().getHeight() > rhs.getBounds().getHeight();
  };
  sort( mImages.begin(), mImages.end(), rev_height_compare );
  Vec2i loc( 0, 0 );
  Vec2i padding( 1, 1 );
  int bottom_y = 0;
  for( ImageData &sprite : mImages )
  {
    if( loc.x + sprite.getBounds().getWidth() > mWidth )
    {
      loc.y = bottom_y + padding.y;
      loc.x = 0;
    }
    sprite.setLoc( loc );
    loc.x += sprite.getBounds().getWidth() + padding.x;
    bottom_y = math<int>::max( sprite.getBounds().getHeight() + loc.y, bottom_y );
  }
  mHeight = bottom_y;
}

void ImagePacker::draw()
{
  for( ImageData &sprite : mImages )
  {
    sprite.draw();
  }
}




