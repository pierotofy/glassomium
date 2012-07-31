=begin
   Glassomium - web-based TUIO-enabled window manager
   http://www.glassomium.org

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

# Configuration file

require './PTCommon/ArgumentParser.rb'

module PTCommon

  class Arguments

    # Set default values, then merge with command line arguments, then create accessors for each
    def initialize(defaults)
      conf = defaults
       params = ArgumentParser.parse(ARGV)
       conf = conf.merge(params);

      # Define runtime accessors method for each entry
       conf.each { |key, value|
         self.class.send(:define_method, key.gsub(/-/,"_")) {
           value
         }
       }
    end
  end
end