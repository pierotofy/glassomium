require 'yaml'
require 'find'

class PictureScanner
	def initialize(delegate)
		@delegate = delegate
		@config = YAML.load_file("#{File.dirname(__FILE__)}/../media.cfg")
		@thread = nil
		@running = false

		@platform = :posix
		if RUBY_PLATFORM =~ /(win|w)32$/
			@platform = :windows
		end

		@cached_files_count = {}

		# Read initial count of files for each mount point
		@config[:watch_points][@platform].each do |directory|
			@cached_files_count[directory] = Dir.glob(File.join(directory, '*')).length
		end
	end

	def start
		@running = true
		@thread = Thread.new{loop()}
		@thread.abort_on_exception = true
	end

	def stop
		@running = false
	end

	def loop
		while @running
			# Scan each mount point for the current platform
			@config[:watch_points][@platform].each do |directory|	

			  #First check whether the number of files inside it has changed
			  current_files_count = Dir.glob(File.join(directory, '*')).length
			  previous_files_count = @cached_files_count[directory]

			  if current_files_count != previous_files_count
			  	puts "PictureScanner: detected change on #{directory}" if $g_verbose

			  	@cached_files_count[directory] = current_files_count

			  	# Search directory for filter (first level ONLY)
			  	filters = @config[:directory_filters]
			  	images = []
			  	extensions = @config[:extensions]

			  	# First check root folder
			  	extensions.each do |ext|
					current_images = Dir.glob(File.join(directory, "*.#{ext}"))
					current_images.each do |image|
						# Take away mount point
						images.push(image.gsub(/^#{directory}/, "/")) 
					end
				end

			  	# Then scan for subdirectories that match our filters
				Dir.glob(File.join(directory, '*')).each do |subdir|
					if FileTest.directory?(subdir)
						match = false
						filters.each do |filter|
							match = true if subdir=~/^.*\/#{Regexp.quote(filter)}$/
						end

						# Found, scan for all images and return the paths
						if match
							extensions.each do |ext|
								current_images = Dir.glob(File.join(subdir, "**", "*.#{ext}"))
								current_images.each do |image|
									# Take away mount point
									images.push(image.gsub(/^#{directory}/, "/")) 
								end
							end
						else
							# Not found, next
						end
					end
				end

				puts "PictureScanner: found #{images}"
				@delegate.on_picture_list_changed(directory, images)
			  	break
			  end
			end

			sleep(2)
		end
	end
end