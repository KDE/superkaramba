# Service Groups demo
# Written by Luke Kenneth Casson Leighton <lkcl@lkcl.net>

# This theme is demonstrates how to parse service groups.
# it's done recursively to give a complete dump of your menu
# that you will also be able to double-check against kmenuedit
# and also the KDE Start button.
#
# for a real-live usage, see kroller.sez (try searching kde-look.org),
# which fires off a sub-theme with the service-group menu options when
# you click on a service group on the "main" bar.

#this import statement allows access to the karamba functions
import karamba

# simple function to display

def create_text(widget, line, depth, disp, name=None, command=None, icon=None):

    x = depth*30
    y = line*7
    width = 400
    height = 8
    drop_txt = karamba.createText(widget, x, y, width, height, disp)
    karamba.changeTextSize(widget, drop_txt, 7)
    karamba.changeTextColor(widget, drop_txt, 252,252,252)

    # create a (very narrow!  only an 8-pixel-high area!)
    # service click area to demonstrate that commands get
    # executed correctly.

    if name:
        karamba.createServiceClickArea( widget,
                                        x,y,width,height,
                         name, command, icon)


# okay.  you really should be looking up the KDE Developer documentation
# on service groups.  here is a _brief_ explanation of what they contain.
#
# getServiceGroups returns a list of menu options.
# you can either have a subgroup, or you can have an executable program.
# if a list item is a subgroup, then (0, dict) is returned, where
# dict is a dictionary containing information describing the subgroup.
# if a list item is an executable menu option, then (1, dict) is returned,
# where dict is a dictionary containing information describing the menu option.
#
# the information (keys in the dictionary) returned about a subgroup comprises:
# relpath - the fully qualified path of that subgroup, and
#           calling getServiceGroups (again) with that path will get
#           you all of the menu options in that subgroup, hurrah.
# icon    - the name of an icon (a png, usually) which you can, if you
#           so wish, use as the filename argument to createImage().
# caption - i get confused as to which is which, but
# and       you can always run this program in combination
# comment   with kmenuedit to find out!
#
# the information (keys in the dictionary) returned about an
# executable option comprises:
# exec    - obviously the most important bit: the name of the command
#           to be executed.
# icon    - the filename of the icon (usually a png) which should be
#           displayed for this command.
# name    - not to be confused with genericname, neither of which i
#           am sure about, and if you really want to know, run
#           kmenuedit and experiment, or look at the KDE Developer source
#           code.
# menuid  - don't know what it is for.  it's just... there.
#
# how to use the service group info:
#
# the simplest way to get commands actually executed is to hand
# over the name, the command and the icon filename to a
# ServiceClickArea (karamba.createServiceClickArea()).
#
# other than making sure that you put the service click area in
# the right place, you don't have to worry about much else.
#
# or, you could always run it manually, yourself, by using
# karamba.Run(name, command, icon).  personally, i prefer
# the service click areas.  no, karamba.Run() is not like
# karamba.execute() - if you use karamba.Run() you get to say which
# bouncing icon is used, whereas if you use karamba.execute() you
# always get a superkaramba bouncing icon...
#
# do look out for kroller.sez on http://kde-look.org or other locations
# for a more complex and comprehensive usage of service groups.

def display_svc_group(widget, rel_path, line, depth):

    grps = karamba.getServiceGroups(rel_path)

    for (type, d) in grps:

        if type is 0:

            # it's a

            sub_relpath = d['relpath']
            icon = d.get('icon', 'unknown')
            caption = d.get('caption', None)
            comment = d.get('comment', None)
            cmt = ''

            # get at least something to display
            if comment:
                cmt  = comment
            elif caption:
                cmt  = caption

            msg = "SVCGRP: %s    %s" % (icon, cmt)
            create_text(widget, line, depth, msg)
            line += 1
            line = display_svc_group(widget, sub_relpath,
                                     line, depth+1)

        elif type is 1 and d.has_key('menuid'):

            relpath = d.get('relpath', '')
            cmd = d['exec']
            icon = d.get('icon', 'unknown')
            name = d.get('name', None)
            genericname = d.get('genericname', None)
            cmt = ''

            # get at least something to display
            if genericname:
                cmt  = genericname
            elif name:
                cmt  = name

            msg = "%s    %s     %s" % (cmd, icon, cmt)
            create_text(widget, line, depth, msg, name, cmd, icon)
            line += 1

    return line

#this is called when you widget is initialized
def initWidget(widget):

    display_svc_group(widget, "", 0, 0)
    karamba.redrawWidget(widget)

