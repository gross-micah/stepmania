#ifndef SCREEN_GAMEPLAY_H
#define SCREEN_GAMEPLAY_H

#include "ScreenWithMenuElements.h"
#include "Sprite.h"
#include "Transition.h"
#include "BitmapText.h"
#include "RageSound.h"
#include "LocalizedString.h"
#include "BeginnerHelper.h"
#include "LyricDisplay.h"
#include "Attack.h"
#include "NetworkSyncManager.h"
#include "AutoKeysounds.h"
#include "ThemeMetric.h"
#include "PlayerStageStats.h"
#include "PlayerState.h"
#include "InputEventPlus.h"
#include "SoundEffectControl.h"
#include "GameplayAssist.h"

class LyricsLoader;
class ActiveAttackList;
class CombinedLifeMeter;
class Player;
class LifeMeter;
class ScoreDisplay;
class StepsDisplay;
class Inventory;
class ScoreKeeper;
class Background;
class Foreground;

AutoScreenMessage( SM_NotesEnded );
AutoScreenMessage( SM_BeginFailed );
AutoScreenMessage( SM_LeaveGameplay );

class PlayerInfo
{
public:
	PlayerInfo();
	~PlayerInfo();

	void Load( PlayerNumber pn, MultiPlayer mp, bool bShowNoteField, int iAddToDifficulty );
	void LoadDummyP1( int iDummyIndex, int iAddToDifficulty );

	/** @brief The player has lost all of their lives: show the special game over. */
	void ShowOniGameOver();
	/**
	 * @brief Retrieve the player's state and stage stats index.
	 * @return the player's state and stage stats index.
	 */
	MultiPlayer GetPlayerStateAndStageStatsIndex()	{ return m_pn == PLAYER_INVALID ? m_mp : (MultiPlayer)m_pn; }
	PlayerState *GetPlayerState();
	PlayerStageStats *GetPlayerStageStats();
	PlayerNumber GetStepsAndTrailIndex()		{ return m_pn == PLAYER_INVALID ? PLAYER_1 : m_pn; }
	/**
	 * @brief Determine if the player information is enabled.
	 * @return its success or failure. */
	bool IsEnabled();
	/**
	 * @brief Determine if we're in MultiPlayer.
	 * @return true if it is MultiPlayer, false otherwise. */
	bool IsMultiPlayer() const { return m_mp != MultiPlayer_Invalid; }
	RString GetName() const
	{
		if( m_bIsDummy )
			return ssprintf("Dummy%d",m_iDummyIndex);
		if( IsMultiPlayer() ) 
			return MultiPlayerToString( m_mp );
		else
			return PlayerNumberToString( m_pn );
	}

	// Lua
	void PushSelf( lua_State *L );

	/** @brief The present Player that is playing the game. */
	PlayerNumber		m_pn;
	MultiPlayer		m_mp;
	bool			m_bIsDummy;
	int			m_iDummyIndex;
	int			m_iAddToDifficulty;	// if > 0, use the Nth harder Steps
	bool			m_bPlayerEnabled; // IsEnabled cache for iterators
	PlayerState		m_PlayerStateDummy;
	PlayerStageStats	m_PlayerStageStatsDummy;
	SoundEffectControl	m_SoundEffectControl;

	/**
	 * @brief The list of Steps a player has to go through in this set.
	 *
	 * The size may be greater than 1 if playing a course. */
	vector<Steps*>		m_vpStepsQueue;
	/**
	 * @brief The list of attack modifiers a player has to go through in this set.
	 *
	 * The size may be greater than 1 if playing a course. */
	vector<AttackArray>	m_asModifiersQueue;

	/** @brief The LifeMeter showing a Player's health. */
	LifeMeter		*m_pLifeMeter;
	/** @brief The current Song number in a Course. */
	BitmapText		*m_ptextCourseSongNumber;
	/** @brief The description of the current Steps. */
	BitmapText		*m_ptextStepsDescription;

	/** @brief The display for the primary ScoreKeeper. */
	ScoreDisplay		*m_pPrimaryScoreDisplay;
	/** @brief The display for the secondary ScoreKeeper. */
	ScoreDisplay		*m_pSecondaryScoreDisplay;
	/** @brief The primary ScoreKeeper for keeping track of the score. */
	ScoreKeeper		*m_pPrimaryScoreKeeper;
	/** @brief The secondary ScoreKeeper. Only used in PLAY_MODE_RAVE. */
	ScoreKeeper		*m_pSecondaryScoreKeeper;
	/** @brief The current PlayerOptions that are activated. */
	BitmapText		*m_ptextPlayerOptions;
	/** @brief The current attack modifiers that are in play for the moment. */
	ActiveAttackList	*m_pActiveAttackList;

	/** @brief The NoteData the Player has to get through. */
	NoteData		m_NoteData;
	Player			*m_pPlayer;

	/**
	 * @brief The inventory of attacks.
	 *
	 * This is mainly used in PLAY_MODE_BATTLE. */
	Inventory		*m_pInventory;

	StepsDisplay	*m_pStepsDisplay;

	AutoActor		m_sprOniGameOver;
};

/** @brief The music plays, the notes scroll, and the Player is pressing buttons. */
class ScreenGameplay : public ScreenWithMenuElements
{
public:
	ScreenGameplay();
	virtual void Init();
	virtual ~ScreenGameplay();
	virtual void BeginScreen();

	virtual void Update( float fDeltaTime );
	virtual void Input( const InputEventPlus &input );
	virtual void HandleScreenMessage( const ScreenMessage SM );
	virtual void HandleMessage( const Message &msg );
	virtual void Cancel( ScreenMessage smSendWhenDone );

	/**
	 * @brief Retrieve the current ScreenType.
	 * @return the gameplay ScreenType. */
	virtual ScreenType GetScreenType() const { return gameplay; }

	/**
	 * @brief Determine if we are to center the columns for just one player.
	 * @return true if we center the solo player, false otherwise. */
	bool Center1Player() const;

	// Lua
	virtual void PushSelf( lua_State *L );
	Song *GetNextCourseSong() const;
	LifeMeter *GetLifeMeter( PlayerNumber pn );
	PlayerInfo *GetPlayerInfo( PlayerNumber pn );
	PlayerInfo *GetDummyPlayerInfo( int iDummyIndex );
	void Pause(bool bPause) { PauseGame(bPause); }
	bool IsPaused() const { return m_bPaused; }

protected:
	virtual void UpdateStageStats( MultiPlayer mp ) {};	// overridden for multiplayer

	virtual bool UseSongBackgroundAndForeground() const { return true; }

	ThemeMetric<RString> PLAYER_TYPE;
	ThemeMetric<RString> SCORE_DISPLAY_TYPE;
	ThemeMetric<apActorCommands> PLAYER_INIT_COMMAND;
	LocalizedString GIVE_UP_START_TEXT;
	LocalizedString GIVE_UP_BACK_TEXT;
	LocalizedString GIVE_UP_ABORTED_TEXT;
	ThemeMetric<float> MUSIC_FADE_OUT_SECONDS;
	ThemeMetric<float> OUT_TRANSITION_LENGTH;
	ThemeMetric<float> COURSE_TRANSITION_LENGTH;
	ThemeMetric<float> BEGIN_FAILED_DELAY;
	ThemeMetric<float> MIN_SECONDS_TO_STEP;
	ThemeMetric<float> MIN_SECONDS_TO_MUSIC;
	ThemeMetric<float> MIN_SECONDS_TO_STEP_NEXT_SONG;
	ThemeMetric<bool> START_GIVES_UP;
	ThemeMetric<bool> BACK_GIVES_UP;
	ThemeMetric<bool> GIVING_UP_GOES_TO_PREV_SCREEN;
	ThemeMetric<int> FAIL_ON_MISS_COMBO;
	ThemeMetric<bool> ALLOW_CENTER_1_PLAYER;
	ThemeMetric<bool> USE_ALTERNATIVE_INPUT;
	ThemeMetric<bool> UNPAUSE_WITH_START;

	vector<AlternateMapping> m_vAlterMap;

	bool IsLastSong();
	void SetupSong( int iSongIndex );
	void ReloadCurrentSong();
	virtual void LoadNextSong();
	void StartPlayingSong( float fMinTimeToNotes, float fMinTimeToMusic );
	void GetMusicEndTiming( float &fSecondsToStartFadingOutMusic, float &fSecondsToStartTransitioningOut );
	void LoadLights();
	void PauseGame( bool bPause, GameController gc = GameController_Invalid );
	void PlayAnnouncer( RString type, float fSeconds );
	void UpdateLights();
	void SendCrossedMessages();
	void BeginBackingOutFromGameplay();
	float GetHasteRate();

	void PlayTicks();
	void UpdateSongPosition( float fDeltaTime );
	void UpdateLyrics( float fDeltaTime );
	void SongFinished();
	virtual void SaveStats();
	virtual void StageFinished( bool bBackedOut );
	void SaveReplay();
	//bool LoadReplay();
	bool AllAreFailing();

	virtual void InitSongQueues();

	enum DancingState { 
		STATE_INTRO = 0, // not allowed to press Back
		STATE_DANCING,
		STATE_OUTRO,	// not allowed to press Back
		NUM_DANCING_STATES
	} m_DancingState;
	bool			m_bPaused;

	GameController		m_PauseController;
	/**
	 * @brief The songs left to play.
	 *
	 * The size can be greater than 1 if playing a course. */
	vector<Song*>		m_apSongsQueue;

	float			m_fTimeSinceLastDancingComment;	// this counter is only running while STATE_DANCING

	LyricDisplay		m_LyricDisplay;

	Background		*m_pSongBackground;
	Foreground		*m_pSongForeground;

	/** @brief Used between songs in a course to show the next song. */
	Transition		m_NextSong;

	CombinedLifeMeter*	m_pCombinedLifeMeter;

	BitmapText		m_textSongOptions;
	BitmapText		m_Scoreboard[NUM_NSScoreBoardColumn];	// for NSMAN, so we can have a scoreboard

	bool			m_bShowScoreboard;

	BitmapText		m_textDebug;

	RageTimer		m_GiveUpTimer;
	void AbortGiveUp( bool bShowText );

	Transition		m_Ready;
	Transition		m_Go;
	Transition		m_Failed;
	Transition		m_Toasty;	// easter egg

	/**
	 * @brief How much time has the player survived in the extra stage?
	 *
	 * TODO: Move this into a BGA. */
	BitmapText		m_textSurviveTime;


	AutoKeysounds		m_AutoKeysounds;

	RageSound		m_soundBattleTrickLevel1;
	RageSound		m_soundBattleTrickLevel2;
	RageSound		m_soundBattleTrickLevel3;

	bool			m_bZeroDeltaOnNextUpdate;

	GameplayAssist		m_GameplayAssist;
	RageSound		*m_pSoundMusic;

	BeginnerHelper		m_BeginnerHelper;

	NoteData		m_CabinetLightsNoteData;

	vector<PlayerInfo>	m_vPlayerInfo;	// filled by SGameplay derivatives in FillPlayerInfo
	virtual void FillPlayerInfo( vector<PlayerInfo> &vPlayerInfoOut ) = 0;
	virtual PlayerInfo &GetPlayerInfoForInput( const InputEventPlus& iep )  { return m_vPlayerInfo[iep.pn]; }

	RageTimer		m_timerGameplaySeconds;
};

vector<PlayerInfo>::iterator GetNextEnabledPlayerInfo		( vector<PlayerInfo>::iterator iter, vector<PlayerInfo> &v );
vector<PlayerInfo>::iterator GetNextEnabledPlayerInfoNotDummy	( vector<PlayerInfo>::iterator iter, vector<PlayerInfo> &v );
vector<PlayerInfo>::iterator GetNextEnabledPlayerNumberInfo	( vector<PlayerInfo>::iterator iter, vector<PlayerInfo> &v );
vector<PlayerInfo>::iterator GetNextPlayerNumberInfo		( vector<PlayerInfo>::iterator iter, vector<PlayerInfo> &v );
vector<PlayerInfo>::iterator GetNextVisiblePlayerInfo		( vector<PlayerInfo>::iterator iter, vector<PlayerInfo> &v );

/** @brief Get each enabled Player's info. */
#define FOREACH_EnabledPlayerInfo( v, pi )		for( vector<PlayerInfo>::iterator pi = GetNextEnabledPlayerInfo		(v.begin(),v);	pi != v.end(); pi = GetNextEnabledPlayerInfo(++pi,v) )
/** @brief Get each enabled Player's info as long as it's not a dummy player. */
#define FOREACH_EnabledPlayerInfoNotDummy( v, pi )	for( vector<PlayerInfo>::iterator pi = GetNextEnabledPlayerInfoNotDummy	(v.begin(),v);	pi != v.end(); pi = GetNextEnabledPlayerInfoNotDummy(++pi,v) )
/** @brief Get each enabled Player Number's info. */
#define FOREACH_EnabledPlayerNumberInfo( v, pi )	for( vector<PlayerInfo>::iterator pi = GetNextEnabledPlayerNumberInfo	(v.begin(),v);	pi != v.end(); pi = GetNextEnabledPlayerNumberInfo(++pi,v) )
/** @brief Get each Player Number's info, regardless of whether it's enabled or not. */
#define FOREACH_PlayerNumberInfo( v, pi )		for( vector<PlayerInfo>::iterator pi = GetNextPlayerNumberInfo		(v.begin(),v);	pi != v.end(); pi = GetNextPlayerNumberInfo(++pi,v) )
/** @brief Get each visible Player's info. */
#define FOREACH_VisiblePlayerInfo( v, pi )		for( vector<PlayerInfo>::iterator pi = GetNextVisiblePlayerInfo		(v.begin(),v);	pi != v.end(); pi = GetNextVisiblePlayerInfo(++pi,v) )


#endif

/**
 * @file
 * @author Chris Danford, Glenn Maynard (c) 2001-2004
 * @section LICENSE
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
