/*
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
*/
   
#include "stdafx.h"
#include <SFML/OpenGL.hpp>
#define GL_BGRA 0x80E1

// Fix a compilation issue on windows
#include <SFML/Graphics/RenderStates.hpp>
const sf::RenderStates sf::RenderStates::Default;

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace Berkelium;

class GLTextureWindow : public Berkelium::WindowDelegate {
public:
    GLTextureWindow(unsigned int _w, unsigned int _h, bool _usetrans)
     : width(_w),
       height(_h),
       needs_full_refresh(true)
    {
		texture = new sf::Texture();
		texture->create(_w, _h);

        scroll_buffer = new char[width*(height+1)*4];
        
		Berkelium::Context *context = Berkelium::Context::create();
        bk_window = Berkelium::Window::create(context);
        delete context;
        bk_window->setDelegate(this);
        bk_window->resize(width, height);
        bk_window->setTransparent(_usetrans);
    }

    ~GLTextureWindow() {
        delete scroll_buffer;
        delete bk_window;
    }

	void loadURL(const std::string& url){
		bk_window->navigateTo(url.data(), url.length());
	}

    Berkelium::Window* getWindow() {
        return bk_window;
    }

    virtual void onPaint(Berkelium::Window *wini,
        const unsigned char *bitmap_in, const Berkelium::Rect &bitmap_rect,
        size_t num_copy_rects, const Berkelium::Rect *copy_rects,
        int dx, int dy, const Berkelium::Rect &scroll_rect) {

	   const int kBytesPerPixel = 4;

		// If we've reloaded the page and need a full update, ignore updates
		// until a full one comes in. This handles out of date updates due to
		// delays in event processing.
		if (needs_full_refresh) {

			// Ignore partial ones
			if (bitmap_rect.left() != 0 || bitmap_rect.top() != 0 || bitmap_rect.right() != width || bitmap_rect.bottom() != height) {
				return;
			}
      
			// Here's our full refresh
			texture->update((Uint8 *)bitmap_in, width, height, 0, 0);

			needs_full_refresh = false;
			return;
		}


		// Now, we first handle scrolling. We need to do this first since it
		// requires shifting existing data, some of which will be overwritten by
		// the regular dirty rect update.
		if (dx != 0 || dy != 0) {

			// scroll_rect contains the Rect we need to move
			// First we figure out where the the data is moved to by translating it
			Berkelium::Rect scrolled_rect = scroll_rect.translate(-dx, -dy);

			// Next we figure out where they intersect, giving the scrolled
			// region
			Berkelium::Rect scrolled_shared_rect = scroll_rect.intersect(scrolled_rect);

			// Only do scrolling if they have non-zero intersection
			if (scrolled_shared_rect.width() > 0 && scrolled_shared_rect.height() > 0) {

				// And the scroll is performed by moving shared_rect by (dx,dy)
				Berkelium::Rect shared_rect = scrolled_shared_rect.translate(dx, dy);

				int wid = scrolled_shared_rect.width();
				int hig = scrolled_shared_rect.height();

				int inc = 1;
				char *outputBuffer = scroll_buffer;
				// source data is offset by 1 line to prevent memcpy aliasing
				// In this case, it can happen if dy==0 and dx!=0.
				char *inputBuffer = scroll_buffer+(width*1*kBytesPerPixel);
				int jj = 0;
				if (dy > 0) {

					// Here, we need to shift the buffer around so that we start in the
					// extra row at the end, and then copy in reverse so that we
					// don't clobber source data before copying it.
					outputBuffer = scroll_buffer+((scrolled_shared_rect.top()+hig+1)*width - hig*wid)*kBytesPerPixel;
					inputBuffer = scroll_buffer;
					inc = -1;
					jj = hig-1;
				}

				// Copy the data out of the texture
				texture->bind(sf::Texture::Pixels);
				glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, inputBuffer);
			
				// Annoyingly, OpenGL doesn't provide convenient primitives, so
				// we manually copy out the region to the beginning of the
				// buffer
				for(; jj < hig && jj >= 0; jj+=inc) {
					memcpy(
						outputBuffer + (jj*wid) * kBytesPerPixel,
						inputBuffer + ((scrolled_shared_rect.top()+jj)*width + scrolled_shared_rect.left()) * kBytesPerPixel,
						wid*kBytesPerPixel
					);
				}

				// And finally, we push it back into the texture in the right
				// location
				texture->update((Uint8 *)outputBuffer, shared_rect.width(), shared_rect.height(), shared_rect.left(), shared_rect.top());
			}
		}

		for (size_t i = 0; i < num_copy_rects; i++) {
			int wid = copy_rects[i].width();
			int hig = copy_rects[i].height();
			int top = copy_rects[i].top() - bitmap_rect.top();
			int left = copy_rects[i].left() - bitmap_rect.left();

			for(int jj = 0; jj < hig; jj++) {
				memcpy(
					scroll_buffer + jj*wid*kBytesPerPixel,
					bitmap_in + (left + (jj+top)*bitmap_rect.width())*kBytesPerPixel,
					wid*kBytesPerPixel
					);
			}

			// Finally, we perform the main update, just copying the rect that is
			// marked as dirty but not from scrolled data.
			texture->update((Uint8 *)scroll_buffer, copy_rects[i].width(), copy_rects[i].height(), copy_rects[i].left(), copy_rects[i].top());
		}

		needs_full_refresh = false;
    }

    virtual void onLoadingStateChanged(Berkelium::Window *win, bool isLoading) {
        std::cout << (void*)win << "*** onLoadingStateChanged "
                  << (isLoading?"started":"stopped") << std::endl;
    }

    Berkelium::Window* window() const {
        return bk_window;
    }

	sf::Texture *getTexture(){ return texture; }

private:
    // The Berkelium window, i.e. our web page
    Berkelium::Window* bk_window;
    // Width and height of our window.
    unsigned int width, height;
    // Bool indicating when we need to refresh the entire image
    bool needs_full_refresh;
    // Buffer used to store data for scrolling
    char* scroll_buffer;

	sf::Texture *texture;
};

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef _WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char *argv[])
#endif
{

    // Create the main rendering window
    RenderWindow App(sf::VideoMode(800, 600, 32), "SFML Graphics");

	sf::Clock clock;

    // Load the sprite image from a file
	sf::Texture t;
	if (!t.loadFromFile("pointer_red.png"))
        return EXIT_FAILURE;

    // Create the sprite

	sf::Texture tex;
	tex.create(10, 10);
	unsigned int *data = new unsigned int[10*10 + 1];
	for (int i = 0; i < 10 * 10; i++){
		data[i] = 0x00FF00FF;
	}
	tex.update((Uint8 *)data);

	//sf::Sprite Sprite3(tex);
	//Sprite3.setPosition(20,20);

	if (!Berkelium::init(FileString::empty())) {
        std::cout << "Failed to initialize berkelium!" << std::endl;
        return 1;
    }

	GLTextureWindow glw(600, 400, true);
	sf::Sprite Sprite(*glw.getTexture());
	glw.loadURL("http://www.youtube.com/watch?v=QGJuMBdaqIw&ob=av3e");
	//glw.loadURL("http://www.reddit.com");
	Sprite.setPosition(20, 20);

	GLTextureWindow glw2(300, 100, true);
	sf::Sprite Sprite2(*glw2.getTexture());
	glw2.loadURL("http://www.reddit.com");
	Sprite2.setPosition(500, 300);

	App.setFramerateLimit(60);

	// Start game loop
	while (App.isOpen())
    {
        // Process events
        sf::Event e;
		while (App.pollEvent(e))
        {
            // Close window : exit
			if (e.type == sf::Event::Closed){
				Berkelium::destroy();
                App.close();
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		 {
						glw.getWindow()->mouseWheel(2, 0);
		 }
		 else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		 {
			glw.getWindow()->mouseWheel(-2, 0);
		 }
		 else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		 {
			//Sprite.move(0, -100 * ElapsedTime);
			 glw.getWindow()->mouseWheel(0, 2);
		 }	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		 {
			 glw.getWindow()->mouseWheel(0, -2);
			//Sprite.move(0,  100 * ElapsedTime);
		 }

		Berkelium::update();

		// Clear screen
		App.clear();

		// Display sprite in our window

		App.draw(Sprite);
		App.draw(Sprite2);
		//App.draw(Sprite3);

		// Display window contents on screen
		App.display();
    }

    return EXIT_SUCCESS;
}


