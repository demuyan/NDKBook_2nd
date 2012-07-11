# -*- coding: utf-8 -*-

#
#cutcode.rb  指定した箇所の一部を切り取りファイルに保存する
#
#
#  (使用例)
#
#  /////begin <ファイル名>
#
#    この間のテキストを<ファイル名>で切り出す
#
#  /////end
#

if ARGV.length < 2
  p "cutcode.rb <input-soucecode> <output directory>"
  exit 1
end

filename = ARGV.shift
outdir = ARGV.shift

outoutflg = false
text = ""
outfilename = ""
open(filename).each do |line|
  dat = /^\/\/\/\/\/begin\s(.*)/.match(line)
  unless dat.nil?
    outoutflg = true
    outfilename = dat[1]
    text = ""
    next
  end

  dat = /^\/\/\/\/\/end/.match(line)
  unless dat.nil?
    outoutflg = false
    File.open(outdir + "/" + outfilename+".txt","w") do |fp|
      fp << text
    end
    next
  end

  if outoutflg
    text << line
  end
end

