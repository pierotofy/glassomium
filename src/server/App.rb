class App
	attr_accessor :app_url, :icon_url, :configuration

	def initialize(app_url, icon_url, configuration)
		@app_url = app_url
		@icon_url = icon_url
		@configuration = configuration
	end
end
