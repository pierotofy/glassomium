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
              GLA._pushTouchEvent("touchstart", -1, event.pageX, event.pageY);
              GLA._fireTouchEvent("touchstart");
            });
            $jsafe(document).mouseup(function(event) { 
              GLA._devMouseDown = false;
              GLA._pushTouchEvent("touchend", -1, event.pageX, event.pageY);
              GLA._fireTouchEvent("touchend");
            });
            $jsafe(document).mousemove(function(event) { 
              if (GLA._devMouseDown){
                GLA._pushTouchEvent("touchmove", -1, event.pageX, event.pageY);
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
