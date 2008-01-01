/*
 *  Copyright (c) 2007 Cyrille Berger (cberger@cberger.net)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "bigbrother.h"

#include <stdlib.h>

#include <kaction.h>
#include <kactioncollection.h>
#include <kcomponentdata.h>
#include <kfiledialog.h>
#include <kgenericfactory.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include <kis_action_recorder.h>
#include <kis_config.h>
#include <kis_cursor.h>
#include <kis_debug.h>
#include <kis_global.h>
#include <kis_image.h>
#include <kis_recorded_action.h>
#include <kis_recorded_action_factory_registry.h>
#include <kis_types.h>
#include <kis_view2.h>

typedef KGenericFactory<BigBrotherPlugin> BigBrotherPluginFactory;
K_EXPORT_COMPONENT_FACTORY( kritabigbrother, BigBrotherPluginFactory( "krita" ) )


BigBrotherPlugin::BigBrotherPlugin(QObject *parent, const QStringList &)
    : KParts::Plugin(parent), m_recorder(0)
{
    if ( parent->inherits("KisView2") )
    {
        m_view = (KisView2*) parent;

        setComponentData(BigBrotherPluginFactory::componentData());

        setXMLFile(KStandardDirs::locate("data","kritaplugins/bigbrother.rc"), true);

        KAction* action = 0;
        // Open and play action
        action  = new KAction(KIcon("media-playback-start"), i18n("Open and play..."), this);
        actionCollection()->addAction("Recording_Open_Play", action );
        connect(action, SIGNAL(triggered()), this, SLOT(slotOpenPlay()));
        // Save recorded action
        action  = new KAction(i18n("Save all actions"), this);
        actionCollection()->addAction("Recording_Global_Save", action );
        connect(action, SIGNAL(triggered()), this, SLOT(slotSave()));
        // Start recording action
        m_startRecordingMacroAction = new KAction(KIcon("media-record"), i18n("Start recording macro"), this);
        actionCollection()->addAction("Recording_Start_Recording_Macro", m_startRecordingMacroAction );
        connect(m_startRecordingMacroAction, SIGNAL(triggered()), this, SLOT(slotStartRecordingMacro()));
        // Save recorded action
        m_stopRecordingMacroAction  = new KAction(KIcon("media-playback-stop"), i18n("Stop recording actions"), this);
        actionCollection()->addAction("Recording_Stop_Recording_Macro", m_stopRecordingMacroAction );
        connect(m_stopRecordingMacroAction, SIGNAL(triggered()), this, SLOT(slotStopRecordingMacro()));
        m_stopRecordingMacroAction->setEnabled( false );
    }
}

BigBrotherPlugin::~BigBrotherPlugin()
{
    m_view = 0;
    delete m_recorder;
}

void BigBrotherPlugin::slotSave()
{
    saveMacro( m_view->image()->actionRecorder() );
}

void BigBrotherPlugin::slotOpenPlay()
{
    QString filename = KFileDialog::getOpenFileName(KUrl(), "*.krarec|Recorded actions (*.krarec)", m_view);
    if(not filename.isNull())
    {
        QDomDocument doc;
        QFile f(filename);
        if(f.exists())
        {
            dbgPlugins << f.open( QIODevice::ReadOnly);
            QString err;
            int line, col;
            if(not doc.setContent(&f, &err, &line, &col))
            {
                // TODO error message
                dbgPlugins << err << " line = " << line << " col = " << col;
                f.close();
                return;
            }
            f.close();
            QDomElement docElem = doc.documentElement();
            if(not docElem.isNull() and docElem.tagName() == "RecordedActions")
            {
                dbgPlugins << "Load the macro";
                KisMacro m(m_view->image());
                m.fromXML(docElem);
                dbgPlugins << "Play the macro";
                m.play();
                dbgPlugins << "Finished";
            } else {
                // TODO error message
            }
        } else {
            dbgPlugins << "Unexistant file : " << filename;
        }
    }
}

void BigBrotherPlugin::slotStartRecordingMacro()
{
    dbgPlugins << "Start recording macro";
    if( m_recorder ) return;
    // Alternate actions
    m_startRecordingMacroAction->setEnabled( false );
    m_stopRecordingMacroAction->setEnabled( true );
    
    // Create recorder
    m_recorder = new KisMacro(m_view->image());
    connect(m_view->image()->actionRecorder(), SIGNAL(addedAction(const KisRecordedAction&)), m_recorder, SLOT(addAction(const KisRecordedAction&)));
}

void BigBrotherPlugin::slotStopRecordingMacro()
{
    dbgPlugins << "Stop recording macro";
    if( not m_recorder ) return;
    // Alternate actions
    m_startRecordingMacroAction->setEnabled( true );
    m_stopRecordingMacroAction->setEnabled( false );
    // Save the macro
    saveMacro( m_recorder );
    // Delete recorder
    delete m_recorder;
    m_recorder = 0;
}

void BigBrotherPlugin::saveMacro(const KisMacro* macro)
{
    QString filename = KFileDialog::getSaveFileName(KUrl(), "*.krarec|Recorded actions (*.krarec)", m_view);
    if(not filename.isNull())
    {
        QDomDocument doc;
        QDomElement e = doc.createElement("RecordedActions");
        
        macro->toXML(doc, e);
        
        doc.appendChild(e);
        QFile f(filename);
        f.open( QIODevice::WriteOnly);
        QTextStream stream(&f);
        doc.save(stream,2);
        f.close();
    }
}

#include "bigbrother.moc"
