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
  require 'webrick'
  require './ThemeManager'
  require './Servlets/Common'

  class BaseServlet < WEBrick::HTTPServlet::AbstractServlet
   
    def initialize(server)
      super(server)     
    end

    def do_GET(request, response)
      response.status = 200
      response['Content-Type'] = "text/html"
      result = ""

      # Get development stuff if flags are enabled
      result += Servlets::Common::getDevelopmentCode(request)

      # Get the actual stuff
      result += get_body(request, response)

      # Escape and one line response always at the end of processing
      if (request.query['response_on_one_line'] == '1')
        result.gsub!(/\r?\n?/, "")
        result.gsub!(/<\/script>/i, "<\\/script>")
      end

      if (request.query['escape_double_quotes'] == '1')
        result.gsub!(/"/, '\"')
      end

      response.body = result
    end

    def do_POST(request, response)
      do_GET(request, response)
    end

    def get_body(request, response)
    end

  end
end
