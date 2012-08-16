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

   # Mount servlet plugins (if any)
   $g_apps.each do |app_name, app|
      if app.servlet_file
        puts "Found servlet plugin for #{app_name}" if $g_verbose

        require "./#{app.servlet_file[0..-4]}"

        servlet_plugin = ServletPlugin.new
        servlet_plugin.get_servlet_handlers.each do |mount_name, handler|
          @webrick.mount "/apps/#{app_name}/#{mount_name}", handler
          puts "Mounted plugin at /apps/#{app_name}/#{mount_name}" if $g_verbose
        end
      end
   end
  end
  
  def start
    @webrick.start;
  end
  
end
