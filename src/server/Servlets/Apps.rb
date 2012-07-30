
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
