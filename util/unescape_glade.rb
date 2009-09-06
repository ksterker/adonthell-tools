#!/bin/env ruby

i=0;
$stdin.readlines.each { |line|
	# First "
	line=line.strip
	#puts line[0..0]
	line=line[1..-1] if line[0..0]=='"'
	line=line[0..-2] if line[-1..-1]=='"'
	line.gsub!('\\\\', '\\')
	line.gsub!('\\"', '"')
	puts line
}

