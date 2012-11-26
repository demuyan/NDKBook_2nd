# -*- coding: utf-8 -*-
require 'rubygems'
require 'cairo'

format = Cairo::FORMAT_ARGB32
width = 256
height = 128

surface = Cairo::ImageSurface.new(format, width, height)
context = Cairo::Context.new(surface)

# 背景
context.set_source_rgba(0, 0, 0, 0) 
context.rectangle(0, 0, width, height)
context.fill


# 円を書く

context.set_source_color(Cairo::Color::GREEN)
r = 15
context.arc(width-16, 16, r, 0, 2 * Math::PI)
context.fill_preserve


# text string
context.set_source_rgb(1, 1, 1)
context.select_font_face("Takao Gothic")
context.set_font_size( 32 )

x = 0
y = height-32
total_width = 0
span_width = 2

hash = Hash.new

0.upto(0x7f) do |i|
  hash[i] = "{0,0,0,0},  /* #{i.chr} */ \n"
end


"0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ+-/*:$,.()=".each_byte { |chr|

  extents = context.text_extents(chr.chr)
  
  if total_width+ extents.x_advance + span_width > width then
    total_width = 0
    x = 0
    y -= 32
  end

  context.move_to(x,y+32)
  context.show_text( chr.chr )

  hash[chr] = "{#{x.to_i},#{(height-32-y).to_i},#{extents.width.to_i},#{extents.height.to_i},#{extents.x_bearing.to_i},#{extents.y_bearing.to_i},#{extents.x_advance.to_i}}, /* #{chr.chr} */\n"

  x = x + extents.x_advance + span_width
  total_width += extents.x_advance + span_width
  


}

File.open("glyphpos.h","w") do |fp|

  str =  <<-EOS
    typedef struct {
       int x;
       int y;
       int width;
       int height;
       int x_bearing;
       int y_bearing;
       int x_advance;
    } glyph;

    glyph glyphlist[] = {
  EOS

  fp << str

  0.upto(0x7f) do |i|
    fp << hash[i]
  end

  str =  <<-EOS2
  };
  EOS2

  fp << str


end

surface.write_to_png("fonttexture.png")

