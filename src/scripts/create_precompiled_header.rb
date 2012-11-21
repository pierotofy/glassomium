#!/usr/bin/ruby1.9.1

#Usage: ./program <precompiled_header_filename> <command_to_generate_header>

puts "Checking for precompiled header..."

if (File.exists?(ARGV[0])) then
	puts "Precompiled header found, no need to rebuild."	
else
	puts "#{ARGV[0]} not found. Generating..."

	# Splits up commands that have include in the form: -I/lib;/lib2 (makes bash execute multiple commands)
	command = ARGV[1].gsub(/;/,' -I') 

	# Eliminates backslashes that we don't need
	command = command.gsub(/\\/,'')
	
	puts "Executing: #{command}"
	system(command)
end
