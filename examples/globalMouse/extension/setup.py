from distutils.core import setup, Extension

module1 = Extension('xcursor', 
		    include_dirs = ['/usr/X11R6/include'],
		    libraries = ['X11'],
		    library_dirs = ['/usr/X11R6/lib'],
		    sources = ['xcursor.c'])

setup (name = 'XMouseCursor',
	version = '1.0',
	description = 'Determines the position of the X mouse cursor',
	ext_modules = [module1])
