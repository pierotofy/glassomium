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

module Servlets
  class Common
    def self.getDevelopmentCode(request)
      result = ""

      # Append initialization code from Glassomium
      if (request.query['development'] == '1')
        jsFile = "JS/injectOnLoad.js"
        if File.exists?(jsFile)
          result += "<script>" + File.read(jsFile) + "</script>";
        else
          raise "Cannot find file #{jsFile}"
        end   

        # And also development code (fixes runtime errors on a web browser)
        devFile = "JS/developmentCode.js"
        if File.exists?(devFile)
          result += "<script>" + File.read(devFile) + "</script>";
        else
          raise "Cannot find file #{devFile}"
        end        
      end

      # Simulate tuio events with jquery
      if (request.query['development'] == '1' && request.query['simulate_tuio'] == '1')
        result += <<-eos
          <script>
            GLA._devMouseDown = false;

            $jsafe(document).mousedown(function(event) {
              GLA._devMouseDown = true;
              GLA._pushTouchEvent("touchstart", -1, event.pageX, event.pageY, 0, 0, 0);
              GLA._fireTouchEvent("touchstart");
            });
            $jsafe(document).mouseup(function(event) { 
              GLA._devMouseDown = false;
              GLA._pushTouchEvent("touchend", -1, event.pageX, event.pageY, 0, 0, 0);
              GLA._fireTouchEvent("touchend");
            });
            $jsafe(document).mousemove(function(event) { 
              if (GLA._devMouseDown){
                GLA._pushTouchEvent("touchmove", -1, event.pageX, event.pageY, 0, 0, 0);
                GLA._fireTouchEvent("touchmove");
              }
            });
          </script>
        eos
      end

      result
    end
  end
end
