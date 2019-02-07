/*
 * Hydrogen
 * Copyright(c) 2002-2008 by Alex >Comix< Cominu [comix@users.sourceforge.net]
 *
 * http://www.hydrogen-music.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <hydrogen/config.h>
#include <hydrogen/version.h>
#include <hydrogen/hydrogen.h>
#include <hydrogen/event_queue.h>
#include <hydrogen/fx/LadspaFX.h>
#include <hydrogen/Preferences.h>
#include <hydrogen/helpers/filesystem.h>

#include "HydrogenApp.h"
#include "Skin.h"
#include "PreferencesDialog.h"
#include "MainForm.h"
#include "PlayerControl.h"
#include "AudioEngineInfoForm.h"
#include "HelpBrowser.h"
#include "LadspaFXProperties.h"
#include "InstrumentRack.h"
#include "Director.h"

#include "PatternEditor/PatternEditorPanel.h"
#include "InstrumentEditor/InstrumentEditorPanel.h"
#include "SoundLibrary/SoundLibraryPanel.h"
#include "SongEditor/SongEditor.h"
#include "SongEditor/SongEditorPanel.h"
#include "SoundLibrary/SoundLibraryDatastructures.h"
#include "PlaylistEditor/PlaylistDialog.h"
#include "SampleEditor/SampleEditor.h"
#include "Mixer/Mixer.h"
#include "Mixer/MixerLine.h"
#include "UndoActions.h"

#include "Widgets/InfoBar.h"


#include <QtGui>
#if QT_VERSION >= 0x050000
#  include <QtWidgets>
#endif


using namespace H2Core;


HydrogenApp* HydrogenApp::m_pInstance = NULL;
const char* HydrogenApp::__class_name = "HydrogenApp";

HydrogenApp::HydrogenApp( MainForm *pMainForm, Song *pFirstSong )
 : Object( __class_name )
 , m_pMainForm( pMainForm )
 , m_pMixer( NULL )
 , m_pPatternEditorPanel( NULL )
 , m_pAudioEngineInfoForm( NULL )
 , m_pSongEditorPanel( NULL )
 , m_pHelpBrowser( NULL )
 , m_pFirstTimeInfo( NULL )
 , m_pPlayerControl( NULL )
 , m_pPlaylistDialog( NULL )
 , m_pSampleEditor( NULL )
 , m_pDirector( NULL )

{
	m_pInstance = this;

	m_pEventQueueTimer = new QTimer(this);
	connect( m_pEventQueueTimer, SIGNAL( timeout() ), this, SLOT( onEventQueueTimer() ) );
	m_pEventQueueTimer->start(50);	// update at 20 fps


	// Create the audio engine :)
	Hydrogen::create_instance();
	Hydrogen::get_instance()->setSong( pFirstSong );
	Preferences::get_instance()->setLastSongFilename( pFirstSong->get_filename() );
	SoundLibraryDatabase::create_instance();

	//setup the undo stack
	m_pUndoStack = new QUndoStack( this );

	updateWindowTitle();

	Preferences *pPref = Preferences::get_instance();

	setupSinglePanedInterface();

	// restore audio engine form properties
	m_pAudioEngineInfoForm = new AudioEngineInfoForm( 0 );
	WindowProperties audioEngineInfoProp = pPref->getAudioEngineInfoProperties();
	m_pAudioEngineInfoForm->move( audioEngineInfoProp.x, audioEngineInfoProp.y );
	if ( audioEngineInfoProp.visible ) {
		m_pAudioEngineInfoForm->show();
	}
	else {
		m_pAudioEngineInfoForm->hide();
	}

	m_pPlaylistDialog = new PlaylistDialog( 0 );
	m_pDirector = new Director( 0 );
}



HydrogenApp::~HydrogenApp()
{
	INFOLOG( "[~HydrogenApp]" );
	m_pEventQueueTimer->stop();


	//delete the undo tmp directory
	cleanupTemporaryFiles();

	delete m_pHelpBrowser;
	delete m_pAudioEngineInfoForm;
	delete m_pMixer;
	delete m_pPlaylistDialog;
	delete m_pDirector;
	delete m_pSampleEditor;

	delete SoundLibraryDatabase::get_instance();

	Hydrogen *pEngine = Hydrogen::get_instance();
	if (pEngine) {
		H2Core::Song * pSong = pEngine->getSong();
		// Hydrogen calls removeSong on from its destructor, so here we just delete the objects:
		delete pEngine;
		delete pSong;
	}

	#ifdef H2CORE_HAVE_LADSPA
	for (uint nFX = 0; nFX < MAX_FX; nFX++) {
		delete m_pLadspaFXProperties[nFX];
	}
	#endif

}



/// Return an HydrogenApp m_pInstance
HydrogenApp* HydrogenApp::get_instance() {
	if (m_pInstance == NULL) {
		std::cerr << "Error! HydrogenApp::get_instance (m_pInstance = NULL)" << std::endl;
	}
	return m_pInstance;
}




void HydrogenApp::setupSinglePanedInterface()
{
	Preferences *pPref = Preferences::get_instance();
	int uiLayout = pPref->getDefaultUILayout();

	// MAINFORM
	WindowProperties mainFormProp = pPref->getMainFormProperties();
	m_pMainForm->resize( mainFormProp.width, mainFormProp.height );
	m_pMainForm->move( mainFormProp.x, mainFormProp.y );

	m_pSplitter = new QSplitter( NULL );
	m_pSplitter->setOrientation( Qt::Vertical );
	m_pSplitter->setOpaqueResize( true );

    m_pTab = new QTabWidget( NULL );

	// SONG EDITOR
	if( uiLayout == Preferences::UI_LAYOUT_SINGLE_PANE)
		m_pSongEditorPanel = new SongEditorPanel( m_pSplitter );
	else
		m_pSongEditorPanel = new SongEditorPanel( m_pTab );

	WindowProperties songEditorProp = pPref->getSongEditorProperties();
	m_pSongEditorPanel->resize( songEditorProp.width, songEditorProp.height );

	if( uiLayout == Preferences::UI_LAYOUT_TABBED)
		m_pTab->addTab( m_pSongEditorPanel, trUtf8("Song Editor") );

	// this HBox will contain the InstrumentRack and the Pattern editor
    QWidget *pSouthPanel = new QWidget( m_pSplitter );
    QHBoxLayout *pEditorHBox = new QHBoxLayout();
    QWidget *pNorthPanel = new QWidget( m_pSplitter );
    QHBoxLayout *pEditorHBoxNorth = new QHBoxLayout();

    if( uiLayout == Preferences::UI_LAYOUT_TABBED ){

        pEditorHBox->setSpacing( 5 );
        pEditorHBox->setMargin( 0 );
        pSouthPanel->setLayout( pEditorHBox );

        pEditorHBoxNorth->setSpacing( 5 );
        pEditorHBoxNorth->setMargin( 0 );
        pNorthPanel->setLayout( pEditorHBoxNorth );
    }else
    {
        pEditorHBox->setSpacing( 5 );
        pEditorHBox->setMargin( 0 );
        pSouthPanel->setLayout( pEditorHBox );
    }


    // Pattern Editor
	m_pInstrumentRack = new InstrumentRack( NULL );


    // Split Soundlibrary Panel On Tabbed Interface
    m_pSoundLibraryPanel = new SoundLibraryPanel( NULL, false );
    m_pSoundLibraryPanel->setMaximumWidth(350);

	if( uiLayout == Preferences::UI_LAYOUT_TABBED ){
		m_pTab->setMovable( false );
		m_pTab->setTabsClosable( false );
        m_pTab->addTab( pSouthPanel, trUtf8( "Pattern + Sound Library") );
	}

    // INSTRUMENT RACK
    if( uiLayout == Preferences::UI_LAYOUT_TABBED ){
        m_pTab->setMovable( false );
        m_pTab->setTabsClosable( false );
        m_pTab->addTab( pNorthPanel, trUtf8( "Instrument") );
    }

    // SOUNDLIBRARY RACK
//    if( uiLayout == Preferences::UI_LAYOUT_TABBED ){
//        m_pTab->setMovable( false );
//        m_pTab->setTabsClosable( false );
//        m_pTab->addTab( m_pSoundLibraryPanel, trUtf8( "Sound Library") );
//    }

	// PATTERN EDITOR
	m_pPatternEditorPanel = new PatternEditorPanel( NULL );
	WindowProperties patternEditorProp = pPref->getPatternEditorProperties();
    m_pPatternEditorPanel->resize( patternEditorProp.width, patternEditorProp.height );
    QRect rec = QApplication::desktop()->screenGeometry();
    if( rec.width() < 1000 ) m_pPatternEditorPanel->setMaximumWidth(600);


    if( uiLayout == Preferences::UI_LAYOUT_TABBED ){
        pEditorHBox->addWidget( m_pPatternEditorPanel );
        pEditorHBox->addWidget( m_pSoundLibraryPanel );
        pEditorHBoxNorth->addWidget( m_pInstrumentRack );
    }else
    {
        pEditorHBox->addWidget( m_pPatternEditorPanel );
        pEditorHBox->addWidget( m_pInstrumentRack );
    }

	// PLayer control
	m_pPlayerControl = new PlayerControl( NULL );


	QWidget *mainArea = new QWidget( m_pMainForm );	// this is the main widget
	m_pMainForm->setCentralWidget( mainArea );

	// LAYOUT!!
	QVBoxLayout *pMainVBox = new QVBoxLayout();
	pMainVBox->setSpacing( 1 );
	pMainVBox->setMargin( 0 );
	pMainVBox->addWidget( m_pPlayerControl );

	m_pInfoBar = new InfoBar();
	m_pInfoBar->hide();
	pMainVBox->addWidget( m_pInfoBar );
	pMainVBox->addSpacing( 3 );

	if( uiLayout == Preferences::UI_LAYOUT_SINGLE_PANE)
		pMainVBox->addWidget( m_pSplitter );
	else {
		pMainVBox->addWidget( m_pTab );

	}

	mainArea->setLayout( pMainVBox );




	// MIXER
	m_pMixer = new Mixer(0);
	WindowProperties mixerProp = pPref->getMixerProperties();

	m_pMixer->resize( mixerProp.width, mixerProp.height );
	m_pMixer->move( mixerProp.x, mixerProp.y );

	if( uiLayout == Preferences::UI_LAYOUT_TABBED){
		m_pTab->addTab(m_pMixer,trUtf8("Mixer"));
	}

	m_pMixer->updateMixer();

	if ( mixerProp.visible && uiLayout == Preferences::UI_LAYOUT_SINGLE_PANE ) {
		m_pMixer->show();
	}
	else {
		m_pMixer->hide();
	}


	// HELP BROWSER
	QString sDocPath = H2Core::Filesystem::doc_dir();
	QString sDocURI = sDocPath + "/manual.html";
	m_pHelpBrowser = new SimpleHTMLBrowser( NULL, sDocPath, sDocURI, SimpleHTMLBrowser::MANUAL );

#ifdef H2CORE_HAVE_LADSPA
	// LADSPA FX
	for (uint nFX = 0; nFX < MAX_FX; nFX++) {
		m_pLadspaFXProperties[nFX] = new LadspaFXProperties( NULL, nFX );
		m_pLadspaFXProperties[nFX]->hide();
		WindowProperties prop = pPref->getLadspaProperties(nFX);
		m_pLadspaFXProperties[nFX]->move( prop.x, prop.y );
		if ( prop.visible ) {
			m_pLadspaFXProperties[nFX]->show();
		}
		else {
			m_pLadspaFXProperties[nFX]->hide();
		}
	}
#endif

	if( uiLayout == Preferences::UI_LAYOUT_TABBED){
		m_pTab->setCurrentIndex( Preferences::get_instance()->getLastOpenTab() );
		QObject::connect(m_pTab, SIGNAL(currentChanged(int)),this,SLOT(currentTabChanged(int)));
	}
}



void HydrogenApp::currentTabChanged(int index)
{
	Preferences::get_instance()->setLastOpenTab( index );
}

void HydrogenApp::closeFXProperties()
{
#ifdef H2CORE_HAVE_LADSPA
	for (uint nFX = 0; nFX < MAX_FX; nFX++) {
		m_pLadspaFXProperties[nFX]->close();
	}
#endif
}

void HydrogenApp::setSong(Song* song)
{
	Hydrogen::get_instance()->setSong( song );
	Preferences::get_instance()->setLastSongFilename( song->get_filename() );

	m_pSongEditorPanel->updateAll();
	m_pPatternEditorPanel->updateSLnameLabel();

	updateWindowTitle();

	m_pMainForm->updateRecentUsedSongList();
}

void HydrogenApp::showMixer(bool show)
{
	/*
		 *   Switch to Mixer tab with alt+m in tabbed mode,
		 *   otherwise open mixer window
		 */

	Preferences *pPref = Preferences::get_instance();
	int uiLayout = pPref->getDefaultUILayout();

	if( uiLayout == Preferences::UI_LAYOUT_TABBED )
	{
		m_pTab->setCurrentIndex( 2 );
	} else {
		m_pMixer->setVisible( show );
	}

	m_pMainForm->update_mixer_checkbox();
}

void HydrogenApp::showInstrumentPanel(bool show)
{
	/*
		 *   Switch to pattern editor/instrument tab in tabbed mode,
		 *   otherwise hide instrument panel
		 */

	Preferences *pPref = Preferences::get_instance();
	int uiLayout = pPref->getDefaultUILayout();

	if( uiLayout == Preferences::UI_LAYOUT_TABBED )
	{
		m_pTab->setCurrentIndex( 1 );
		getInstrumentRack()->setHidden( show );
	} else {
		getInstrumentRack()->setHidden( show );
	}
		m_pMainForm->update_instrument_checkbox( !show );
}



void HydrogenApp::showPreferencesDialog()
{
	PreferencesDialog preferencesDialog(m_pMainForm);
	preferencesDialog.exec();
}




void HydrogenApp::setStatusBarMessage( const QString& msg, int msec )
{
	if(msg.length()>9){
		setScrollStatusBarMessage( msg, 1500);
		return;
	}
	getPlayerControl()->resetStatusLabel();
	getPlayerControl()->showMessage( msg, msec );

}

void HydrogenApp::updateWindowTitle()
{
	Song *pSong = 	Hydrogen::get_instance()->getSong();
	assert(pSong);

	QString title;

	// special handling for initial title
	QString qsSongName( pSong->__name );

	if( qsSongName == "Untitled Song" && !pSong->get_filename().isEmpty() ){
		qsSongName = pSong->get_filename().section( '/', -1 );
	}

	if(pSong->get_is_modified()){
		title = qsSongName + " (" + QString(trUtf8("modified")) + ")";
	} else {
		title = qsSongName;
	}

	m_pMainForm->setWindowTitle( ( "Hydrogen " + QString( get_version().c_str()) + QString( " - " ) + title ) );
}

void HydrogenApp::setScrollStatusBarMessage( const QString& msg, int msec, bool test )
{
	getPlayerControl()->showScrollMessage( msg, msec , test);
}



void HydrogenApp::showAudioEngineInfoForm()
{
	m_pAudioEngineInfoForm->hide();
	m_pAudioEngineInfoForm->show();
}

void HydrogenApp::showPlaylistDialog()
{
	if ( m_pPlaylistDialog->isVisible() ) {
		m_pPlaylistDialog->hide();
	} else {
		m_pPlaylistDialog->show();
	}
	m_pMainForm->update_playlist_checkbox();
}


void HydrogenApp::showDirector()
{
	if ( m_pDirector->isVisible() ) {
		m_pDirector->hide();
	} else {
		m_pDirector->show();
	}
	m_pMainForm->update_director_checkbox();
}


void HydrogenApp::showSampleEditor( QString name, int mSelectedComponemt, int mSelectedLayer )
{

	if ( m_pSampleEditor ){
		QApplication::setOverrideCursor(Qt::WaitCursor);
		m_pSampleEditor->close();
		delete m_pSampleEditor;
		m_pSampleEditor = NULL;
		QApplication::restoreOverrideCursor();
	}
	QApplication::setOverrideCursor(Qt::WaitCursor);
	m_pSampleEditor = new SampleEditor( 0, mSelectedComponemt, mSelectedLayer, name );
	m_pSampleEditor->show();
	QApplication::restoreOverrideCursor();
}

void HydrogenApp::onDrumkitLoad( QString name ){
	setStatusBarMessage( trUtf8( "Drumkit loaded: [%1]" ).arg( name ), 2000 );
	m_pPatternEditorPanel->updateSLnameLabel( );
}

void HydrogenApp::enableDestructiveRecMode(){
	m_pPatternEditorPanel->displayorHidePrePostCB();
}

void HydrogenApp::songModifiedEvent()
{
	updateWindowTitle();
}

void HydrogenApp::onEventQueueTimer()
{
	// use the timer to do schedule instrument slaughter;
	EventQueue *pQueue = EventQueue::get_instance();

	Event event;
	while ( ( event = pQueue->pop_event() ).type != EVENT_NONE ) {
		for (int i = 0; i < (int)m_EventListeners.size(); i++ ) {
			EventListener *pListener = m_EventListeners[ i ];

			switch ( event.type ) {
			case EVENT_STATE:
				pListener->stateChangedEvent( event.value );
				break;

			case EVENT_PATTERN_CHANGED:
				pListener->patternChangedEvent();
				break;

			case EVENT_PATTERN_MODIFIED:
				pListener->patternModifiedEvent();
				break;

			case EVENT_SONG_MODIFIED:
				songModifiedEvent();
				break;

			case EVENT_SELECTED_PATTERN_CHANGED:
				pListener->selectedPatternChangedEvent();
				break;

			case EVENT_SELECTED_INSTRUMENT_CHANGED:
				pListener->selectedInstrumentChangedEvent();
				break;

			case EVENT_PARAMETERS_INSTRUMENT_CHANGED:
				pListener->parametersInstrumentChangedEvent();
				break;

			case EVENT_MIDI_ACTIVITY:
				pListener->midiActivityEvent();
				break;

			case EVENT_NOTEON:
				pListener->noteOnEvent( event.value );
				break;

			case EVENT_ERROR:
				pListener->errorEvent( event.value );
				break;

			case EVENT_XRUN:
				pListener->XRunEvent();
				break;

			case EVENT_METRONOME:
				pListener->metronomeEvent( event.value );
				break;

			case EVENT_RECALCULATERUBBERBAND:
				pListener->rubberbandbpmchangeEvent();
				break;

			case EVENT_PROGRESS:
				pListener->progressEvent( event.value );
				break;

			case EVENT_JACK_SESSION:
				pListener->jacksessionEvent( event.value );
				break;

			case EVENT_PLAYLIST_LOADSONG:
				pListener->playlistLoadSongEvent( event.value );
				break;

			case EVENT_UNDO_REDO:
				pListener->undoRedoActionEvent( event.value );
				break;

			case EVENT_TEMPO_CHANGED:
				pListener->tempoChangedEvent( event.value );
				break;

			default:
				ERRORLOG( QString("[onEventQueueTimer] Unhandled event: %1").arg( event.type ) );
			}

		}
	}

	// midi notes
	while(!pQueue->m_addMidiNoteVector.empty()){

		int rounds = 1;
		if(pQueue->m_addMidiNoteVector[0].b_noteExist)// runn twice, delete old note and add new note. this let the undo stack consistent
			rounds = 2;
		for(int i = 0; i<rounds; i++){
			SE_addNoteAction *action = new SE_addNoteAction( pQueue->m_addMidiNoteVector[0].m_column,
															 pQueue->m_addMidiNoteVector[0].m_row,
															 pQueue->m_addMidiNoteVector[0].m_pattern,
															 pQueue->m_addMidiNoteVector[0].m_length,
															 pQueue->m_addMidiNoteVector[0].f_velocity,
															 pQueue->m_addMidiNoteVector[0].f_pan_L,
															 pQueue->m_addMidiNoteVector[0].f_pan_R,
															 0.0,
															 pQueue->m_addMidiNoteVector[0].nk_noteKeyVal,
															 pQueue->m_addMidiNoteVector[0].no_octaveKeyVal,
															 false,
															 false,
															 pQueue->m_addMidiNoteVector[0].b_isMidi,
															 pQueue->m_addMidiNoteVector[0].b_isInstrumentMode);

			HydrogenApp::get_instance()->m_pUndoStack->push( action );
		}
		pQueue->m_addMidiNoteVector.erase(pQueue->m_addMidiNoteVector.begin());

	}
}


void HydrogenApp::addEventListener( EventListener* pListener )
{
	if (pListener) {
		m_EventListeners.push_back( pListener );
	}
}


void HydrogenApp::removeEventListener( EventListener* pListener )
{
	for ( uint i = 0; i < m_EventListeners.size(); i++ ) {
		if ( pListener == m_EventListeners[ i ] ) {
			m_EventListeners.erase( m_EventListeners.begin() + i );
		}
	}
}

/**
 * Removes temporary files that were created
 * for undo'ing things.
 */
void HydrogenApp::cleanupTemporaryFiles()
{
	Filesystem::rm( Filesystem::tmp_dir(), true );
}
