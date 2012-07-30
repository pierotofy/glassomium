require './Servlets/Launcher'
require './Servlets/ApplicationConfigs'
require './Servlets/ThemeConfig'
require './Servlets/SystemStatic'
require './Servlets/Apps'
require './Servlets/InjectOnLoad'
require './ThemeManager'
require './ServerLogger'


require 'webrick'
include WEBrick

class UIServer
  attr_accessor :webrick

  def initialize(port, address = nil)
   @webrick = HTTPServer.new(:Port => port, :Logger => ServerLogger.new, 
     :DocumentRoot => "./WebRoot", :DoNotReverseLookup => true)
   registerServlets   
   ['INT', 'TERM'].each { |signal|
      trap(signal){ @webrick.shutdown } 
   }
   puts "Ready to process requests on port #{port}"
  end
  
  def registerServlets
   @webrick.mount "/system/launcher", Servlets::Launcher
   @webrick.mount "/system/appconfigs", Servlets::ApplicationConfigs
   @webrick.mount "/system/themeconfig", Servlets::ThemeConfig
   @webrick.mount "/system", Servlets::SystemStatic
   @webrick.mount "/system/injectonload", Servlets::InjectOnLoad
   
   @webrick.mount "/resources", WEBrick::HTTPServlet::FileHandler, "./Themes/#{ThemeManager.getCurrentTheme}/resources"
   @webrick.mount "/apps", Servlets::Apps, "./WebRoot/apps"
  end
  
  def start
    @webrick.start;
  end
  
end
