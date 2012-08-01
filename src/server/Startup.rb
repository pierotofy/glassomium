=begin
   Glassomium - web-based TUIO-enabled window manager
   http://www.glassomium.org

   Copyright 2012 The Glassomium Authors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
=end

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
