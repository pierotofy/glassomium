require './ProgramInfo'
def startup(args)
  puts PROGRAM_NAME + " " + PROGRAM_VERSION if args.v
  
  if args.help
    puts <<EOF
Usage: rps <arguments>

--help               Display this message
-v                   Enable verbose
--version            Display version
--port               Set the port number of the server (default is 8080)
--bindaddr           Specify a bind address for the server (default is 0:0:0:0)
--theme              Select the template to use (valid entries are listed in the Themes directory, default is 'default')
EOF
    exit(0)
  end

  if args.version
    puts PROGRAM_VERSION
    exit(0)
  end
end
