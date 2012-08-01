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

# Argument parser class
# Takes the ARGV list and returns an hashed item of the form:
# { param1 (without -- or -) : value }
module PTCommon
  class ArgumentParser
      def self.parse(args)
        res = {}
        (0..args.length).each { |i|
          if args[i]=~/^--/
            if args[i+1] =~/^-/ || args[i+1] == nil
             res.store(String(args[i]).gsub(/--/, ""), true)
            elsif
             res.store(String(args[i].gsub(/--/, "")), args[i+1])
            end            
          elsif args[i]=~/^-[^-]/
            args[i].each_char { |c|
              next if c == '-'
              res.store(String(c), true)
            }            
          end
        }
        return res
      end
  end
end

