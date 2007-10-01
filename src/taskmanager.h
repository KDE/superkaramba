/*****************************************************************

Copyright (c) 2000-2001 Matthias Elter <elter@kde.org>
Copyright (c) 2001 Richard Moore <rich@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#ifndef __taskmanager_h__
#define __taskmanager_h__

#include <sys/types.h>

#include <netwm.h>

#include <QObject>
#include <QPixmap>
#include <QPoint>
#include <QMimeData>
#include <QMap>
#include <QDrag>
#include <QRect>
#include <QVector>

#include <KSharedPtr>
#include <KStartupInfo>
#include <KWindowSystem>

#if defined(HAVE_XCOMPOSITE) && \
    defined(HAVE_XRENDER) && \
    defined(HAVE_XFIXES)
#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xrender.h>
#include <fixx11h.h>
#if XCOMPOSITE_VERSION >= 00200 && \
    XFIXES_VERSION >= 20000 && \
    (RENDER_MAJOR > 0 || RENDER_MINOR >= 6)
#define THUMBNAILING_POSSIBLE
#endif
#endif

#include "superkaramba_export.h"

class KWindowSystem;
class TaskManager;

typedef QList<WId> WindowList;

/**
 * A dynamic interface to a task (main window).
 *
 * @see TaskManager
 * @see KWinModule
 */
class SUPERKARAMBA_EXPORT Task: public QObject, public KShared
{
    Q_OBJECT
    Q_PROPERTY(QString visibleName READ visibleName)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString visibleNameWithState READ visibleNameWithState)
    Q_PROPERTY(QPixmap pixmap READ pixmap)
    Q_PROPERTY(bool maximized READ isMaximized)
    Q_PROPERTY(bool minimized READ isMinimized)
    // KDE4 deprecated
    Q_PROPERTY(bool iconified READ isIconified)
    Q_PROPERTY(bool shaded READ isShaded WRITE setShaded)
    Q_PROPERTY(bool active READ isActive)
    Q_PROPERTY(bool onCurrentDesktop READ isOnCurrentDesktop)
    Q_PROPERTY(bool onAllDesktops READ isOnAllDesktops)
    Q_PROPERTY(bool alwaysOnTop READ isAlwaysOnTop WRITE setAlwaysOnTop)
    Q_PROPERTY(bool modified READ isModified)
    Q_PROPERTY(bool demandsAttention READ demandsAttention)
    Q_PROPERTY(int desktop READ desktop)
    Q_PROPERTY(double thumbnailSize READ thumbnailSize WRITE setThumbnailSize)
    Q_PROPERTY(bool hasThumbnail READ hasThumbnail)
    Q_PROPERTY(QPixmap thumbnail READ thumbnail)

public:
    typedef KSharedPtr<Task> TaskPtr;
    typedef QVector<Task::TaskPtr> List;
    typedef QMap<WId, Task::TaskPtr> Dict;

    Task(WId win, QObject *parent, const char *name = 0);
    virtual ~Task();

    WId window() const
    {
        return _win;
    }
    KWindowInfo info() const
    {
        return _info;
    }

    QString visibleName() const
    {
        return _info.visibleName();
    }
    QString visibleNameWithState() const
    {
        return _info.visibleNameWithState();
    }
    QString name() const
    {
        return _info.name();
    }
    QString className();
    QString classClass();

    /**
     * A list of the window ids of all transient windows (dialogs) associated
     * with this task.
     */
    WindowList transients() const
    {
        return _transients;
    }

    /**
     * Returns a 16x16 (KIconLoader::Small) icon for the task. This method will
     * only fall back to a static icon if there is no icon of any size in
     * the WM hints.
     */
    QPixmap pixmap() const
    {
        return _pixmap;
    }

    /**
     * Returns the best icon for any of the KIconLoader::StdSizes. If there is no
     * icon of the specified size specified in the WM hints, it will try to
     * get one using KIconLoader.
     *
     * <pre>
     *   bool gotStaticIcon;
     *   QPixmap icon = myTask->icon( KIconLoader::SizeMedium, gotStaticIcon );
     * </pre>
     *
     * @param size Any of the constants in KIconLoader::StdSizes.
     * @param isStaticIcon Set to true if KIconLoader was used, false otherwise.
     */
    QPixmap bestIcon(int size, bool &isStaticIcon);

    /**
     * Tries to find an icon for the task with the specified size. If there
     * is no icon that matches then it will either resize the closest available
     * icon or return a null pixmap depending on the value of allowResize.
     *
     * Note that the last icon is cached, so a sequence of calls with the same
     * parameters will only query the NET properties if the icon has changed or
     * none was found.
     */
    QPixmap icon(int width, int height, bool allowResize = false);

    /**
     * Returns true iff the windows with the specified ids should be grouped
     * together in the task list.
     */
    static bool idMatch(const QString &, const QString &);

    // state

    /**
     * Returns true if the task's window is maximized.
     */
    bool isMaximized() const;

    /**
     * Returns true if the task's window is minimized.
     */
    bool isMinimized() const;

    /**
     * @deprecated
     * Returns true if the task's window is minimized(iconified).
     */
    bool isIconified() const;

    /**
     * Returns true if the task's window is shaded.
     */
    bool isShaded() const;

    /**
     * Returns true if the task's window is the active window.
     */
    bool isActive() const;

    /**
     * Returns true if the task's window is the topmost non-iconified,
     * non-always-on-top window.
     */
    bool isOnTop() const;

    /**
     * Returns true if the task's window is on the current virtual desktop.
     */
    bool isOnCurrentDesktop() const;

    /**
     * Returns true if the task's window is on all virtual desktops.
     */
    bool isOnAllDesktops() const;

    /**
     * Returns true if the task's window will remain at the top of the
     * stacking order.
     */
    bool isAlwaysOnTop() const;

    /**
     * Returns true if the task's window will remain at the bottom of the
     * stacking order.
     */
    bool isKeptBelowOthers() const;

    /**
     * Returns true if the task's window is in full screen mode
     */
    bool isFullScreen() const;

    /**
     * Returns true if the document the task is editing has been modified.
     * This is currently handled heuristically by looking for the string
     * '[i18n_modified]' in the window title where i18n_modified is the
     * word 'modified' in the current language.
     */
    bool isModified() const ;

    /**
     * Returns the desktop on which this task's window resides.
     */
    int desktop() const
    {
        return _info.desktop();
    }

    /**
     * Returns true if the task is not active but demands user's attention.
     */
    bool demandsAttention() const;


    /**
    * Returns true if the window is on the specified screen of a multihead configuration
    */
    bool isOnScreen(int screen) const;

    /**
     * Returns true if the task should be shown in taskbar-like apps
     */
    bool showInTaskbar() const
    {
        return _info.state() ^ NET::SkipTaskbar;
    }

    /**
     * Returns true if the task should be shown in pager-like apps
     */
    bool showInPager() const
    {
        return _info.state() ^ NET::SkipPager;
    }

    /**
     * Returns the geometry for this window
     */
    QRect geometry() const
    {
        return _info.geometry();
    }

    // internal

    //* @internal
    void refresh(unsigned int dirty);
    //* @internal
    void refreshIcon();
    //* @internal
    void addTransient(WId w, const NETWinInfo& info);
    //* @internal
    void removeTransient(WId w);
    //* @internal
    bool hasTransient(WId w) const
    {
        return _transients.indexOf(w) != -1;
    }
    //* @internal
    void updateDemandsAttentionState(WId w);
    //* @internal
    void setActive(bool a);

    // For thumbnails

    /**
     * Returns the current thumbnail size.
     */
    double thumbnailSize() const
    {
        return _thumbSize;
    }

    /**
     * Sets the size for the window thumbnail. For example a size of
     * 0.2 indicates the thumbnail will be 20% of the original window
     * size.
     */
    void setThumbnailSize(double size)
    {
        _thumbSize = size;
    }

    /**
     * Returns true if this task has a thumbnail. Note that this method
     * can only ever return true after a call to updateThumbnail().
     */
    bool hasThumbnail() const
    {
        return !_thumb.isNull();
    }

    /**
     * Returns the thumbnail for this task (or a null image if there is
     * none).
     */
    const QPixmap &thumbnail() const
    {
        return _thumb;
    }

    QPixmap thumbnail(int maxDimension);

    void updateWindowPixmap();

public Q_SLOTS:
    // actions

    /**
     * Maximise the main window of this task.
     */
    void setMaximized(bool);
    void toggleMaximized();

    /**
     * Restore the main window of the task (if it was iconified).
     */
    void restore();

    /**
     * Move the window of this task.
     */
    void move();

    /**
     * Resize the window of this task.
     */
    void resize();

    /**
     * Iconify the task.
     */
    void setIconified(bool);
    void toggleIconified();

    /**
     * Close the task's window.
     */
    void close();

    /**
     * Raise the task's window.
     */
    void raise();

    /**
     * Lower the task's window.
     */
    void lower();

    /**
      * Activate the task's window.
      */
    void activate();

    /**
     * Perform the action that is most appropriate for this task. If it
     * is not active, activate it. Else if it is not the top window, raise
     * it. Otherwise, iconify it.
     */
    void activateRaiseOrIconify();

    /**
     * If true, the task's window will remain at the top of the stacking order.
     */
    void setAlwaysOnTop(bool);
    void toggleAlwaysOnTop();

    /**
     * If true, the task's window will remain at the bottom of the stacking order.
     */
    void setKeptBelowOthers(bool);
    void toggleKeptBelowOthers();

    /**
     * If true, the task's window will enter full screen mode.
     */
    void setFullScreen(bool);
    void toggleFullScreen();

    /**
     * If true then the task's window will be shaded. Most window managers
     * represent this state by displaying on the window's title bar.
     */
    void setShaded(bool);
    void toggleShaded();

    /**
     * Moves the task's window to the specified virtual desktop.
     */
    void toDesktop(int);

    /**
     * Moves the task's window to the current virtual desktop.
     */
    void toCurrentDesktop();

    /**
     * This method informs the window manager of the location at which this
     * task will be displayed when iconised. It is used, for example by the
     * KWindowSystem inconify animation.
     */
    void publishIconGeometry(QRect);

    /**
     * Tells the task to generate a new thumbnail. When the thumbnail is
     * ready the thumbnailChanged() signal will be emitted.
     */
    void updateThumbnail();

Q_SIGNALS:
    /**
     * Indicates that this task has changed in some way.
     */
    void changed();

    /**
     * Indicates that the icon for this task has changed.
     */
    void iconChanged();

    /**
     * Indicates that this task is now the active task.
     */
    void activated();

    /**
     * Indicates that this task is no longer the active task.
     */
    void deactivated();

    /**
     * Indicates that the thumbnail for this task has changed.
     */
    void thumbnailChanged();

protected Q_SLOTS:
    //* @internal
    void generateThumbnail();

protected:
    void findWindowFrameId();

private:
    bool                _active;
    WId                 _win;
    WId                 m_frameId;
    QPixmap             _pixmap;
    KWindowInfo    _info;
    WindowList          _transients;
    WindowList          _transients_demanding_attention;

    int                 _lastWidth;
    int                 _lastHeight;
    bool                _lastResize;
    QPixmap             _lastIcon;

    double _thumbSize;
    QPixmap _thumb;
    QPixmap _grab;
    QRect m_iconGeometry;
#ifdef THUMBNAILING_POSSIBLE
    Pixmap              m_windowPixmap;
#endif // THUMBNAILING_POSSIBLE
};


/**
 * Provids a drag object for tasks across desktops.
 * FIXME: should be folded into the Task class the same way it has been with
 *        AppletInfo and KUrl
 */
class SUPERKARAMBA_EXPORT TaskDrag : public QDrag
{
public:
    /**
     * Constructs a task drag object for a task list.
     */
    explicit TaskDrag(const Task::List& tasks, QWidget* source = 0);
    ~TaskDrag();

    /**
     * Returns true if the mime source can be decoded to a TaskDrag.
     */
    static bool canDecode(const QMimeData* e);

    /**
     * Decodes the tasks from the mime source and returns them if successful.
     * Otherwise an empty task list is returned.
     */
    static Task::List decode(const QMimeData* e);
};


/**
 * Represents a task which is in the process of starting.
 *
 * @see TaskManager
 */
class SUPERKARAMBA_EXPORT Startup: public QObject, public KShared
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text)
    Q_PROPERTY(QString bin READ bin)
    Q_PROPERTY(QString icon READ icon)

public:
    typedef KSharedPtr<Startup> StartupPtr;
    typedef QVector<Startup::StartupPtr> List;

    Startup(const KStartupInfoId& id, const KStartupInfoData& data, QObject * parent,
            const char *name = 0);
    virtual ~Startup();

    /**
     * The name of the starting task (if known).
     */
    QString text() const
    {
        return _data.findName();
    }

    /**
     * The name of the executable of the starting task.
     */
    QString bin() const
    {
        return _data.bin();
    }

    /**
     * The name of the icon to be used for the starting task.
     */
    QString icon() const
    {
        return _data.findIcon();
    }
    void update(const KStartupInfoData& data);
    const KStartupInfoId& id() const
    {
        return _id;
    }

Q_SIGNALS:
    /**
     * Indicates that this startup has changed in some way.
     */
    void changed();

private:
    KStartupInfoId _id;
    KStartupInfoData _data;
    class StartupPrivate *d;
};


/**
 * A generic API for task managers. This class provides an easy way to
 * build NET compliant task managers. It provides support for startup
 * notification, virtual desktops and the full range of WM properties.
 *
 * @see Task
 * @see Startup
 * @see KWinModule
 */
class SUPERKARAMBA_EXPORT TaskManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentDesktop READ currentDesktop)
    Q_PROPERTY(int numberOfDesktops READ numberOfDesktops)

public:
    static TaskManager* self();
    ~TaskManager();

    /**
     * Returns the task for a given WId, or 0 if there is no such task.
     */
    Task::TaskPtr findTask(WId w);

    /**
     * Returns the task for a given location, or 0 if there is no such task.
     */
    Task::TaskPtr findTask(int desktop, const QPoint& p);

    /**
     * Returns a list of all current tasks.
     */
    Task::Dict tasks() const
    {
        return m_tasksByWId;
    }

    /**
     * Returns a list of all current startups.
     */
    Startup::List startups() const
    {
        return _startups;
    }

    /**
     * Returns the name of the nth desktop.
     */
    QString desktopName(int n) const;

    /**
     * Returns the number of virtual desktops.
     */
    int numberOfDesktops() const;

    /**
     * Returns the number of the current desktop.
     */
    int currentDesktop() const;

    /**
     * Returns true if the specified task is on top.
     */
    bool isOnTop(const Task*);

    /**
     * Tells the task manager whether or not we care about geometry
     * updates. This generates a lot of activity so should only be used
     * when necessary.
     */
    void trackGeometry()
    {
        m_trackGeometry = true;
    }

    /**
    * Returns whether the Window with WId wid is on the screen screen
    */
    static bool isOnScreen(int screen, const WId wid);

    KWindowSystem* winModule() const
    {
        return m_winModule;
    }

    void setXCompositeEnabled(bool state);
    static bool xCompositeEnabled()
    {
        return m_xCompositeEnabled != 0;
    }

Q_SIGNALS:
    /**
     * Emitted when the active window changed
     */
    void activeTaskChanged(Task::TaskPtr);

    /**
     * Emitted when a new task has started.
     */
    void taskAdded(Task::TaskPtr);

    /**
     * Emitted when a task has terminated.
     */
    void taskRemoved(Task::TaskPtr);

    /**
     * Emitted when a new task is expected.
     */
    void startupAdded(Startup::StartupPtr);

    /**
     * Emitted when a startup item should be removed. This could be because
     * the task has started, because it is known to have died, or simply
     * as a result of a timeout.
     */
    void startupRemoved(Startup::StartupPtr);

    /**
     * Emitted when the current desktop changes.
     */
    void desktopChanged(int desktop);

    /**
     * Emitted when a window changes desktop.
     */
    void windowChanged(Task::TaskPtr);
    void windowChangedGeometry(Task::TaskPtr);

protected Q_SLOTS:
    //* @internal
    void windowAdded(WId);
    //* @internal
    void windowRemoved(WId);
    //* @internal
    void windowChanged(WId, unsigned int);

    //* @internal
    void activeWindowChanged(WId);
    //* @internal
    void currentDesktopChanged(int);
    //* @internal
    void killStartup(const KStartupInfoId&);
    //* @internal
    void killStartup(Startup::StartupPtr);

    //* @internal
    void gotNewStartup(const KStartupInfoId&, const KStartupInfoData&);
    //* @internal
    void gotStartupChange(const KStartupInfoId&, const KStartupInfoData&);

protected:
    void configure_startup();
    void updateWindowPixmap(WId);

private:
    TaskManager();

    Task::TaskPtr               _active;
    Task::Dict m_tasksByWId;
    WindowList _skiptaskbar_windows;
    Startup::List _startups;
    KStartupInfo* _startup_info;
    KWindowSystem* m_winModule;
    bool m_trackGeometry;

    static TaskManager* m_self;
    static uint m_xCompositeEnabled;

    class TaskManagerPrivate *d;
};

#endif
