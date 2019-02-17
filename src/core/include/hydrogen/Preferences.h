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

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <list>
//#include <string>
#include <vector>
#include <cassert>

#include <hydrogen/midi_action.h>
#include <hydrogen/globals.h>
#include <hydrogen/object.h>

#include <QStringList>
#include <QDomDocument>

namespace H2Core
{

const float FALLOFF_SLOW = 	1.08f;
const float FALLOFF_NORMAL=	1.1f;
const float FALLOFF_FAST =	1.5f;


/**
\ingroup H2CORE
*/
class WindowProperties : public H2Core::Object
{
	H2_OBJECT
public:
	int x;
	int y;
	int width;
	int height;
	bool visible;

	WindowProperties();
	~WindowProperties();

	void set(int _x, int _y, int _width, int _height, bool _visible) {
		x = _x; y = _y;
		width = _width; height = _height;
		visible = _visible;
	}

};


/**
\ingroup H2CORE
*/
class H2RGBColor : public H2Core::Object
{
	H2_OBJECT
public:
	H2RGBColor( int r = -1, int g = -1, int b = -1 );
	H2RGBColor( const QString& sColor );
	~H2RGBColor();

	QString toStringFmt();

	int getRed() const {
		return m_red;
	}
	int getGreen() const {
		return m_green;
	}
	int getBlue() const {
		return m_blue;
	}

private:
	int m_red;
	int m_green;
	int m_blue;

};


/**
\ingroup H2CORE
\brief	Colors for hydrogen
*/
class UIStyle : public H2Core::Object
{
	H2_OBJECT
public:
	UIStyle();
	H2RGBColor m_songEditor_backgroundColor;
	H2RGBColor m_songEditor_alternateRowColor;
	H2RGBColor m_songEditor_selectedRowColor;
	H2RGBColor m_songEditor_lineColor;
	H2RGBColor m_songEditor_textColor;
	H2RGBColor m_songEditor_pattern1Color;

	H2RGBColor m_patternEditor_backgroundColor;
	H2RGBColor m_patternEditor_alternateRowColor;
	H2RGBColor m_patternEditor_selectedRowColor;
	H2RGBColor m_patternEditor_textColor;
	H2RGBColor m_patternEditor_noteColor;
	H2RGBColor m_patternEditor_noteoffColor;
	H2RGBColor m_patternEditor_lineColor;
	H2RGBColor m_patternEditor_line1Color;
	H2RGBColor m_patternEditor_line2Color;
	H2RGBColor m_patternEditor_line3Color;
	H2RGBColor m_patternEditor_line4Color;
	H2RGBColor m_patternEditor_line5Color;
};



/**
\ingroup H2CORE
\brief	Manager for User Preferences File (singleton)
*/
class Preferences : public H2Core::Object
{
	H2_OBJECT
public:
	enum {
		USE_JACK_TRANSPORT = 0,
		USE_JACK_TIME_MASTER = 0,
		POST_FADER = 0,
		SET_PLAY_ON = 0,
		BC_ON = 0,
		NO_JACK_TRANSPORT = 1,
		NO_JACK_TIME_MASTER = 1,
		PRE_FADER = 1,
		SET_PLAY_OFF = 1,
		BC_OFF = 1
	};


	enum UI_LAYOUT_TYPES {
			UI_LAYOUT_SINGLE_PANE,
			UI_LAYOUT_TABBED
	};

	int m_nDefaultUILayout;


	QString m_sPreferencesFilename;
	QString m_sPreferencesDirectory;

	QString __lastspatternDirectory;
	QString __lastsampleDirectory; // audio file browser
	bool __playsamplesonclicking; // audio file browser

	bool __playselectedinstrument; // midi keys and keys play instrument or drumset

	int m_nRecPreDelete; //index of record note pre delete function 0 = off
	int m_nRecPostDelete;

	bool m_bFollowPlayhead;



	// switch to enable / disable lash, only on h2 startup
	bool m_brestartLash;
	bool m_bsetLash;

	//soundlibrarypanel expand song and pattern item
	bool __expandSongItem;
	bool __expandPatternItem;

	//beatcounter
	bool m_bbc;
	bool m_mmcsetplay;

	int m_countOffset;
	int m_startOffset;
	//~ beatcounter

	std::list<QString> sServerList;
	std::list<QString> m_patternCategories;

	//___ audio engine properties ___
	QString m_sAudioDriver;		///< Audio Driver
	bool m_bUseMetronome;		///< Use metronome?
	float m_fMetronomeVolume;	///< Metronome volume FIXME: remove this volume!!
	unsigned m_nMaxNotes;		///< max notes
	unsigned m_nBufferSize;		///< Audio buffer size
	unsigned m_nSampleRate;		///< Audio sample rate

	//___ oss driver properties ___
	QString m_sOSSDevice;		///< Device used for output

	//___ MIDI Driver properties
	QString m_sMidiDriver;
	QString m_sMidiPortName;
	int m_nMidiChannelFilter;
	bool m_bMidiNoteOffIgnore;
	bool m_bMidiDiscardNoteAfterAction;

	//___  alsa audio driver properties ___
	QString m_sAlsaAudioDevice;

	//___  jack driver properties ___
	QString m_sJackPortName1;
	QString m_sJackPortName2;
	int m_bJackTransportMode;
	bool m_bJackConnectDefaults;
	bool m_bJackTrackOuts;
	int m_nJackTrackOutputMode;
	//jack time master
<<<<<<< HEAD
	int m_bJackMasterMode ;
	//~ jack time master
=======
	int					m_bJackMasterMode ;
	//~ jack driver properties

	// rpi pad
	bool				m_rpiConfigWaitforMidiMessages;

>>>>>>> d7fa066... add rpi keyevents
	///Default text editor (used by Playlisteditor)
	QString m_sDefaultEditor;

	///Rubberband CLI
	QString m_rubberBandCLIexecutable;

	/// Returns an instance of PreferencesMng class
	static void create_instance();
	static Preferences* get_instance() { assert(__instance); return __instance; }

	~Preferences();

	/// Load the preferences file
	void loadPreferences( bool bGlobal );

	/// Save the preferences file
	void savePreferences();

	const QString& getDemoPath() {
		return demoPath;
	}
	const QString& getDataDirectory() {
		return m_sDataDirectory;
	}

	const QString& getTmpDirectory(){
		return m_sTmpDirectory;
	}

	const QString& getDefaultEditor() {
		return m_sDefaultEditor;
	}

	void setDefaultEditor( QString editor){
		m_sDefaultEditor = editor;
	}

		int getDefaultUILayout(){
			return m_nDefaultUILayout;
		}

		void setDefaultUILayout( int layout){
				m_nDefaultUILayout = layout;
		}

	void setShowExportWarning( bool value ) {
		m_bShowExportWarning = value;
	}

	bool getShowExportWarning() {
		return m_bShowExportWarning;
	}

	// rpi pad
	void				setRpiConfigWaitforMidiMessages( bool value );
	bool				getRpiConfigWaitforMidiMessages();


	// General
	void setRestoreLastSongEnabled( bool restore ) {
		m_brestoreLastSong = restore;
	}

	void setRestoreLastPlaylistEnabled( bool restore ) {
		m_brestoreLastPlaylist = restore;
	}

	void setShowDevelWarning( bool value ) {
		m_bShowDevelWarning = value;
	}

	bool getShowDevelWarning() {
		return m_bShowDevelWarning;
	}

	bool isRestoreLastSongEnabled() {
		return m_brestoreLastSong;
	}

	bool isRestoreLastPlaylistEnabled() {
		return m_brestoreLastPlaylist;
	}

	void setLastSongFilename( const QString& filename ) {
		m_lastSongFilename = filename;
	}
	const QString& getLastSongFilename() {
		return m_lastSongFilename;
	}

	void setLastPlaylistFilename( const QString& filename ) {
		m_lastPlaylistFilename = filename;
	}
	const QString& getLastPlaylistFilename() {
		return m_lastPlaylistFilename;
	}

	void setHearNewNotes( bool value ) {
		hearNewNotes = value;
	}
	bool getHearNewNotes() {
		return hearNewNotes;
	}

	void setRecordEvents( bool value ) {
		recordEvents = value;
	}
	bool getRecordEvents() {
		return recordEvents;
	}

	void setDestructiveRecord ( bool value ) {
		destructiveRecord = value;
	}
	bool getDestructiveRecord() {
		return destructiveRecord;
	}

	void setPunchInPos ( unsigned pos ) {
		punchInPos = pos;
	}
	int getPunchInPos() {
		return punchInPos;
	}

	void setPunchOutPos ( unsigned pos ) {
		punchOutPos = pos;
	}
	int getPunchOutPos() {
		return punchOutPos;
	}

	bool inPunchArea (int pos) {
		// Return true if punch area not defined
		if ( punchInPos <= punchOutPos ) {
			if ( pos < punchInPos || punchOutPos < pos ) {
				return false;
			}
		}
		return true;
	}

	void unsetPunchArea () {
		punchInPos = 0;
		punchOutPos = -1;
	}

	void setQuantizeEvents( bool value ) {
		quantizeEvents = value;
	}
	bool getQuantizeEvents() {
		return quantizeEvents;
	}

	std::vector<QString> getRecentFiles() {
		return m_recentFiles;
	}
	void setRecentFiles( std::vector<QString> recentFiles );

	QStringList getRecentFX() {
		return m_recentFX;
	}
	void setMostRecentFX( QString );

	std::vector<QString> getLadspaPath() {
		return m_ladspaPathVect;
	}
	void setLadspaPath( std::vector<QString> pathVect ) {
		m_ladspaPathVect = pathVect;
	}

	QString getLastNews() {
		return m_sLastNews;
	}
	void setLastNews( const QString& sNews ) {
		m_sLastNews = sNews;
	}


	// GUI Properties
	const QString& getQTStyle() {
		return m_sQTStyle;
	}
	void setQTStyle( const QString& sStyle ) {
		m_sQTStyle = sStyle;
	}


	const QString& getApplicationFontFamily() {
		return applicationFontFamily;
	}
	void setApplicationFontFamily( const QString& family ) {
		applicationFontFamily = family;
	}

	int getApplicationFontPointSize() {
		return applicationFontPointSize;
	}
	void setApplicationFontPointSize( int size ) {
		applicationFontPointSize = size;
	}

	QString getMixerFontFamily() {
		return mixerFontFamily;
	}
	void setMixerFontFamily( const QString& family ) {
		mixerFontFamily = family;
	}
	int getMixerFontPointSize() {
		return mixerFontPointSize;
	}
	void setMixerFontPointSize( int size ) {
		mixerFontPointSize = size;
	}
	float getMixerFalloffSpeed() {
		return mixerFalloffSpeed;
	}
	void setMixerFalloffSpeed( float value ) {
		mixerFalloffSpeed = value;
	}
	bool showInstrumentPeaks() {
		return m_bShowInstrumentPeaks;
	}
	void setInstrumentPeaks( bool value ) {
		m_bShowInstrumentPeaks = value;
	}

	int getPatternEditorGridResolution() {
		return m_nPatternEditorGridResolution;
	}
	void setPatternEditorGridResolution( int value ) {
		m_nPatternEditorGridResolution = value;
	}

	bool isPatternEditorUsingTriplets() {
		return m_bPatternEditorUsingTriplets;
	}
	void setPatternEditorUsingTriplets( bool value ) {
		m_bPatternEditorUsingTriplets = value;
	}

	bool isFXTabVisible() {
		return m_bIsFXTabVisible;
	}
	void setFXTabVisible( bool value ) {
		m_bIsFXTabVisible = value;
	}

	unsigned getPatternEditorGridHeight() {
		return m_nPatternEditorGridHeight;
	}
	void setPatternEditorGridHeight( unsigned value ) {
		m_nPatternEditorGridHeight = value;
	}

	unsigned getPatternEditorGridWidth() {
		return m_nPatternEditorGridWidth;
	}
	void setPatternEditorGridWidth( unsigned value ) {
		m_nPatternEditorGridWidth = value;
	}

	WindowProperties getMainFormProperties() {
		return mainFormProperties;
	}
	void setMainFormProperties( const WindowProperties& prop ) {
		mainFormProperties = prop;
	}

	WindowProperties getMixerProperties() {
		return mixerProperties;
	}
	void setMixerProperties( const WindowProperties& prop ) {
		mixerProperties = prop;
	}

	WindowProperties getPatternEditorProperties() {
		return patternEditorProperties;
	}
	void setPatternEditorProperties( const WindowProperties& prop ) {
		patternEditorProperties = prop;
	}

	WindowProperties getSongEditorProperties() {
		return songEditorProperties;
	}
	void setSongEditorProperties( const WindowProperties& prop ) {
		songEditorProperties = prop;
	}


	WindowProperties getAudioEngineInfoProperties() {
		return audioEngineInfoProperties;
	}
	void setAudioEngineInfoProperties( const WindowProperties& prop ) {
		audioEngineInfoProperties = prop;
	}

	WindowProperties getLadspaProperties( unsigned nFX ) {
		return m_ladspaProperties[nFX];
	}
	void setLadspaProperties( unsigned nFX, const WindowProperties& prop ) {
		m_ladspaProperties[nFX] = prop;
	}

	UIStyle* getDefaultUIStyle() {
		return m_pDefaultUIStyle;
	}

	bool patternModePlaysSelected() {
		return m_bPatternModePlaysSelected;
	}
	void setPatternModePlaysSelected( bool b ) {
		m_bPatternModePlaysSelected = b;
	}

	bool useLash(){
		return m_bUseLash;
	}
	void setUseLash( bool b ){
		m_bUseLash = b;
	}

	void setMaxBars( int bars ){
		maxBars = bars;
	}

	int getMaxBars(){
		return maxBars;
	}

		void setWaitForSessionHandler(bool value){
			waitingForSessionHandler = value;
		}

		bool getWaitForSessionHandler(){
			return waitingForSessionHandler;
		}

#ifdef H2CORE_HAVE_JACKSESSION
		QString getJackSessionUUID(){
			return jackSessionUUID;
		}

		void setJackSessionUUID( QString uuid ){
			jackSessionUUID = uuid;
		}
		QString getJackSessionApplicationPath(){
			return jackSessionApplicationPath;
		}

		void setJackSessionApplicationPath( QString path ){
			jackSessionApplicationPath = path;
		}

#endif

		bool getUseTimelineBpm(){
			return __useTimelineBpm;
		}
		void setUseTimelineBpm( bool val ){
			__useTimelineBpm = val;
		}

		int getRubberBandCalcTime(){
				return __rubberBandCalcTime;
		}
		void setRubberBandCalcTime( int val ){
				__rubberBandCalcTime = val;
		}

		int getRubberBandBatchMode(){
				return m_useTheRubberbandBpmChangeEvent;
		}
		void setRubberBandBatchMode( int val ){
				m_useTheRubberbandBpmChangeEvent = val;
		}

	int getLastOpenTab(){
		return m_nLastOpenTab;
	}
	void setLastOpenTab(int n){
		m_nLastOpenTab = n;
	}

private:
	static Preferences *__instance;

	QString m_sDataDirectory;
	QString m_sTmpDirectory;

	/** directory of demo songs */
	QString demoPath;

	//___ General properties ___
	int __rubberBandCalcTime;
	bool m_useTheRubberbandBpmChangeEvent; ///rubberband bpm change queue
	bool m_bPatternModePlaysSelected; /// Behaviour of Pattern Mode
	bool m_brestoreLastSong;		///< Restore last song?
	bool m_brestoreLastPlaylist;
	bool m_bUseLash;
	bool m_bShowDevelWarning;	///< Show development version warning?
	bool m_bShowExportWarning;
	QString m_lastSongFilename;	///< Last song used
	QString m_lastPlaylistFilename;
	bool hearNewNotes;
	std::vector<QString> m_recentFiles;
	QStringList m_recentFX;
	std::vector<QString> m_ladspaPathVect;
	bool quantizeEvents;
	bool recordEvents;
	bool destructiveRecord;
	bool readPrefFileforotherplaces;
	int punchInPos;
	int punchOutPos;
	QString m_sLastNews;
	int maxBars;
#ifdef H2CORE_HAVE_JACKSESSION
		QString jackSessionUUID;
		QString jackSessionApplicationPath;
#endif

	bool waitingForSessionHandler;
	bool __useTimelineBpm;

	//___ GUI properties ___
	QString m_sQTStyle;
	int m_nLastOpenTab;

	QString applicationFontFamily;
	int applicationFontPointSize;
	QString mixerFontFamily;
	int mixerFontPointSize;
	float mixerFalloffSpeed;
	int m_nPatternEditorGridResolution;
	bool m_bPatternEditorUsingTriplets;
	bool m_bShowInstrumentPeaks;
	bool m_bIsFXTabVisible;
	unsigned m_nPatternEditorGridHeight;
	unsigned m_nPatternEditorGridWidth;
	WindowProperties mainFormProperties;
	WindowProperties mixerProperties;
	WindowProperties patternEditorProperties;
	WindowProperties songEditorProperties;
	WindowProperties drumkitManagerProperties;
	WindowProperties audioEngineInfoProperties;
	WindowProperties m_ladspaProperties[MAX_FX];

	UIStyle*  m_pDefaultUIStyle;


	Preferences();

	/// Create preferences directory
	void createPreferencesDirectory();

	/// Create data directory
	void createDataDirectory();

	/// Create soundLibrary directory
	void createSoundLibraryDirectories();

	WindowProperties readWindowProperties( QDomNode parent, const QString& windowName, WindowProperties defaultProp );
	void writeWindowProperties( QDomNode parent, const QString& windowName, const WindowProperties& prop );

	void writeUIStyle( QDomNode parent );
	void readUIStyle( QDomNode parent );
};

<<<<<<< HEAD
=======
inline bool Preferences::getStartInFullscreenMode() const
{
	return m_startInFullscreenMode;
}

inline void Preferences::setStartInFullscreenMode(bool val)
{
	m_startInFullscreenMode = val;
}

inline const QString& Preferences::getDemoPath() {
	return demoPath;
}
inline const QString& Preferences::getDataDirectory() {
	return m_sDataDirectory;
}

inline const QString& Preferences::getTmpDirectory(){
	return m_sTmpDirectory;
}

inline const QString& Preferences::getDefaultEditor() {
	return m_sDefaultEditor;
}

inline void Preferences::setDefaultEditor( QString editor){
	m_sDefaultEditor = editor;
}

inline int Preferences::getDefaultUILayout(){
	return m_nDefaultUILayout;
}

inline void Preferences::setDefaultUILayout( int layout){
	m_nDefaultUILayout = layout;
}

inline void Preferences::setShowExportWarning( bool value ) {
	m_bShowExportWarning = value;
}

inline bool Preferences::getShowExportWarning() {
	return m_bShowExportWarning;
}

// rpi pad
inline void	Preferences::setRpiConfigWaitforMidiMessages( bool value ){
	m_rpiConfigWaitforMidiMessages = value;
}

inline bool Preferences::getRpiConfigWaitforMidiMessages(){
	return m_rpiConfigWaitforMidiMessages;
}

// General
inline void Preferences::setRestoreLastSongEnabled( bool restore ) {
	m_brestoreLastSong = restore;
}

inline void Preferences::setRestoreLastPlaylistEnabled( bool restore ) {
	m_brestoreLastPlaylist = restore;
}

inline void Preferences::setUseRelativeFilenamesForPlaylists( bool value ) {
	m_bUseRelativeFilenamesForPlaylists= value;
}

inline void Preferences::setShowDevelWarning( bool value ) {
	m_bShowDevelWarning = value;
}

inline bool Preferences::getShowDevelWarning() {
	return m_bShowDevelWarning;
}

inline bool Preferences::isRestoreLastSongEnabled() {
	return m_brestoreLastSong;
}

inline bool Preferences::isRestoreLastPlaylistEnabled() {
	return m_brestoreLastPlaylist;
}

inline bool Preferences::isPlaylistUsingRelativeFilenames() {
	return m_bUseRelativeFilenamesForPlaylists;
}

inline void Preferences::setLastSongFilename( const QString& filename ) {
	m_lastSongFilename = filename;
}
inline const QString& Preferences::getLastSongFilename() {
	return m_lastSongFilename;
}

inline void Preferences::setLastPlaylistFilename( const QString& filename ) {
	m_lastPlaylistFilename = filename;
}
inline const QString& Preferences::getLastPlaylistFilename() {
	return m_lastPlaylistFilename;
}

inline void Preferences::setHearNewNotes( bool value ) {
	hearNewNotes = value;
}
inline bool Preferences::getHearNewNotes() {
	return hearNewNotes;
}

inline void Preferences::setRecordEvents( bool value ) {
	recordEvents = value;
}
inline bool Preferences::getRecordEvents() {
	return recordEvents;
}

inline void Preferences::setDestructiveRecord ( bool value ) {
	destructiveRecord = value;
}
inline bool Preferences::getDestructiveRecord() {
	return destructiveRecord;
}

inline void Preferences::setPunchInPos ( unsigned pos ) {
	punchInPos = pos;
}
inline int Preferences::getPunchInPos() {
	return punchInPos;
}

inline void Preferences::setPunchOutPos ( unsigned pos ) {
	punchOutPos = pos;
}
inline int Preferences::getPunchOutPos() {
	return punchOutPos;
}

inline bool Preferences::inPunchArea (int pos) {
	// Return true if punch area not defined
	if ( punchInPos <= punchOutPos ) {
		if ( pos < punchInPos || punchOutPos < pos ) {
			return false;
		}
	}
	return true;
}

inline void Preferences::unsetPunchArea () {
	punchInPos = 0;
	punchOutPos = -1;
}

inline void Preferences::setQuantizeEvents( bool value ) {
	quantizeEvents = value;
}
inline bool Preferences::getQuantizeEvents() {
	return quantizeEvents;
}

inline std::vector<QString> Preferences::getRecentFiles() {
	return m_recentFiles;
}

inline QStringList Preferences::getRecentFX() {
	return m_recentFX;
}

inline std::vector<QString> Preferences::getLadspaPath() {
	return m_ladspaPathVect;
}
inline void Preferences::setLadspaPath( std::vector<QString> pathVect ) {
	m_ladspaPathVect = pathVect;
}


// GUI Properties
inline const QString& Preferences::getQTStyle() {
	return m_sQTStyle;
}
inline void Preferences::setQTStyle( const QString& sStyle ) {
	m_sQTStyle = sStyle;
}


inline const QString& Preferences::getApplicationFontFamily() {
	return applicationFontFamily;
}
inline void Preferences::setApplicationFontFamily( const QString& family ) {
	applicationFontFamily = family;
}

inline int Preferences::getApplicationFontPointSize() {
	return applicationFontPointSize;
}
inline void Preferences::setApplicationFontPointSize( int size ) {
	applicationFontPointSize = size;
}

inline QString Preferences::getMixerFontFamily() {
	return mixerFontFamily;
}
inline void Preferences::setMixerFontFamily( const QString& family ) {
	mixerFontFamily = family;
}
inline int Preferences::getMixerFontPointSize() {
	return mixerFontPointSize;
}
inline void Preferences::setMixerFontPointSize( int size ) {
	mixerFontPointSize = size;
}
inline float Preferences::getMixerFalloffSpeed() {
	return mixerFalloffSpeed;
}
inline void Preferences::setMixerFalloffSpeed( float value ) {
	mixerFalloffSpeed = value;
}
inline bool Preferences::showInstrumentPeaks() {
	return m_bShowInstrumentPeaks;
}
inline void Preferences::setInstrumentPeaks( bool value ) {
	m_bShowInstrumentPeaks = value;
}

inline int Preferences::getPatternEditorGridResolution() {
	return m_nPatternEditorGridResolution;
}
inline void Preferences::setPatternEditorGridResolution( int value ) {
	m_nPatternEditorGridResolution = value;
}

inline bool Preferences::isPatternEditorUsingTriplets() {
	return m_bPatternEditorUsingTriplets;
}
inline void Preferences::setPatternEditorUsingTriplets( bool value ) {
	m_bPatternEditorUsingTriplets = value;
}

inline bool Preferences::isFXTabVisible() {
	return m_bIsFXTabVisible;
}
inline void Preferences::setFXTabVisible( bool value ) {
	m_bIsFXTabVisible = value;
}

inline unsigned Preferences::getPatternEditorGridHeight() {
	return m_nPatternEditorGridHeight;
}
inline void Preferences::setPatternEditorGridHeight( unsigned value ) {
	m_nPatternEditorGridHeight = value;
}

inline unsigned Preferences::getPatternEditorGridWidth() {
	return m_nPatternEditorGridWidth;
}
inline void Preferences::setPatternEditorGridWidth( unsigned value ) {
	m_nPatternEditorGridWidth = value;
}

inline void Preferences::setColoringMethodAuxValue( int value ){
	m_nColoringMethodAuxValue = value;
}

inline int Preferences::getColoringMethodAuxValue() const{
	return m_nColoringMethodAuxValue;
}

inline void Preferences::setColoringMethod( int value ){
	m_nColoringMethod = value;
}

inline int Preferences::getColoringMethod() const{
	return m_nColoringMethod;
}

inline WindowProperties Preferences::getMainFormProperties() {
	return mainFormProperties;
}
inline void Preferences::setMainFormProperties( const WindowProperties& prop ) {
	mainFormProperties = prop;
}

inline WindowProperties Preferences::getMixerProperties() {
	return mixerProperties;
}
inline void Preferences::setMixerProperties( const WindowProperties& prop ) {
	mixerProperties = prop;
}

inline WindowProperties Preferences::getPatternEditorProperties() {
	return patternEditorProperties;
}
inline void Preferences::setPatternEditorProperties( const WindowProperties& prop ) {
	patternEditorProperties = prop;
}

inline WindowProperties Preferences::getSongEditorProperties() {
	return songEditorProperties;
}
inline void Preferences::setSongEditorProperties( const WindowProperties& prop ) {
	songEditorProperties = prop;
}


inline WindowProperties Preferences::getAudioEngineInfoProperties() {
	return audioEngineInfoProperties;
}
inline void Preferences::setAudioEngineInfoProperties( const WindowProperties& prop ) {
	audioEngineInfoProperties = prop;
}

inline WindowProperties Preferences::getLadspaProperties( unsigned nFX ) {
	return m_ladspaProperties[nFX];
}
inline void Preferences::setLadspaProperties( unsigned nFX, const WindowProperties& prop ) {
	m_ladspaProperties[nFX] = prop;
}

inline UIStyle* Preferences::getDefaultUIStyle() {
	return m_pDefaultUIStyle;
}

inline bool Preferences::patternModePlaysSelected() {
	return m_bPatternModePlaysSelected;
}
inline void Preferences::setPatternModePlaysSelected( bool b ) {
	m_bPatternModePlaysSelected = b;
}

inline bool Preferences::useLash(){
	return m_bUseLash;
}
inline void Preferences::setUseLash( bool b ){
	m_bUseLash = b;
}

inline void Preferences::setMaxBars( int bars ){
	maxBars = bars;
}

inline int Preferences::getMaxBars(){
	return maxBars;
}

inline void Preferences::setWaitForSessionHandler(bool value){
	waitingForSessionHandler = value;
}

inline bool Preferences::getWaitForSessionHandler(){
		return waitingForSessionHandler;
}

#ifdef H2CORE_HAVE_JACKSESSION
inline QString Preferences::getJackSessionUUID(){
	return jackSessionUUID;
}

inline void Preferences::setJackSessionUUID( QString uuid ){
	jackSessionUUID = uuid;
}

inline QString Preferences::getJackSessionApplicationPath(){
	return jackSessionApplicationPath;
}

inline void Preferences::setJackSessionApplicationPath( QString path ){
	jackSessionApplicationPath = path;
}

#endif


#ifdef H2CORE_HAVE_NSMSESSION
inline void Preferences::setNsmClientId(const QString& nsmClientId){
	m_sNsmClientId = nsmClientId;
}

inline QString Preferences::getNsmClientId(void){
	return m_sNsmClientId;
}

inline void Preferences::setNsmSongName(const QString& nsmSongName){
	m_sNsmSongName = nsmSongName;
}

inline QString Preferences::getNsmSongName(void){
	return m_sNsmSongName;
}

#endif

inline bool Preferences::getUseTimelineBpm(){
	return __useTimelineBpm;
}
inline void Preferences::setUseTimelineBpm( bool val ){
	__useTimelineBpm = val;
}

inline int Preferences::getRubberBandCalcTime(){
	return __rubberBandCalcTime;
}
inline void Preferences::setRubberBandCalcTime( int val ){
	__rubberBandCalcTime = val;
}

inline int Preferences::getRubberBandBatchMode(){
	return m_useTheRubberbandBpmChangeEvent;
}
inline void Preferences::setRubberBandBatchMode( int val ){
	m_useTheRubberbandBpmChangeEvent = val;
}

inline int Preferences::getLastOpenTab(){
	return m_nLastOpenTab;
}
inline void Preferences::setLastOpenTab(int n){
	m_nLastOpenTab = n;
}

inline void Preferences::setH2ProcessName(const QString& processName){
	m_sH2ProcessName = processName;
}

inline QString Preferences::getH2ProcessName() {
	return m_sH2ProcessName;
}



>>>>>>> d7fa066... add rpi keyevents
};

#endif

