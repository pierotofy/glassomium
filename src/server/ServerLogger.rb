require 'webrick'
include WEBrick

class ServerLogger < BasicLog
 
  def log(level, data)
    super(level,data) if $g_verbose
  end
  
  def fatal(msg)
    $stderr.puts msg
    exit(1)
  end
  
  def error(msg)
    $stderr.puts msg
  end
end
