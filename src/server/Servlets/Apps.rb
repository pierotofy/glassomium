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

# Open class to override DefaultFileHandler
# I could not find a better way to inject my own
# code, there are no postprocessing callbacks in webbrick
class WEBrick::HTTPServlet::DefaultFileHandler
  def do_GET(req, res)
    st = File::stat(@local_path)
    mtime = st.mtime

    if req['range']
      make_partial_content(req, res, @local_path, st.size)
      raise HTTPStatus::PartialContent
    else
      devcode = Servlets::Common::getDevelopmentCode(req)
      mtype = HTTPUtils::mime_type(@local_path, @config[:MimeTypes])
      res['content-type'] = mtype
      res['content-length'] = st.size + devcode.length
      res['last-modified'] = mtime.httpdate
      res.body = devcode
      res.body += open(@local_path, "rb").read
    end
  end
end

module Servlets
  require 'webrick'
  require './Servlets/Common'

  class Apps < WEBrick::HTTPServlet::FileHandler
    def initialize(server, path)
      super(server, path)
    end
  end
end
