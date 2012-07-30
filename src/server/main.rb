#!/usr/bin/env ruby

require './PTCommon/Arguments'
require './Startup'
require './UIServer'
require './AppScanner'

args = PTCommon::Arguments.new(
  {
  'port' => "5555",
  'bindaddr' => "127.0.0.1",
  'v' => false, # Verbose mode
  'help' => false,
  'version' => false,
  'theme' => 'default',
  'pauseonfailure' => false,
  })
startup(args)

# Force using 1.9, 1.8 has slow sockets that force a reverse DNS resolution at each connection.
if not RUBY_VERSION =~ /1\.9.*/
  puts "ERROR: You need to run this program with a 1.9 version of Ruby. You are currently using #{RUBY_VERSION}"
  puts ""
  puts "You can fix this error by installing a 1.9.x version of Ruby and making sure your environment variable PATH is pointing to the correct version. On Linux you might want to consider using the rvm package to install and manage multiple versions of Ruby."
  if args.pauseonfailure
    STDIN.gets
  end
  exit(1)
end

$g_verbose = args.v

theme = args.theme

# Check theme directory
if not ThemeManager.themeExists?(theme)
  $stderr.puts "Invalid theme " + theme + ". Make sure the name exists in the Themes/ directory. Exiting..."
  exit(1)
end
ThemeManager.setCurrentTheme(theme)
ThemeManager.verify
puts "Read configuration #{ThemeManager.readConfiguration}" if $g_verbose

$g_apps = AppScanner.scanApps("WebRoot/apps", "http://#{args.bindaddr}:#{args.port}/apps/")

begin
  uiserver = UIServer.new(args.port.to_i, args.bindaddr)
  uiserver.start
rescue Errno::EADDRINUSE
  $stderr.puts "Cannot start server on #{args.bindaddr}:#{args.port} because the port is already used by another program. Exiting..."
  exit(1)
end
