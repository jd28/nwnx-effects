## Status
* Version: v0.4

## Description
* Reimplements ExecuteCommandApplyEffectOnObject to allow for duration
  type DURATION\_TYPE\_EQUIPPED and DURATION\_TYPE\_INNATE
* Hooks and creates NWNX plugin events for effect application and
  removal and item property application and removel.
  
## Change Log
* v0.4
  * Hook CExoArrayList<CGameEffect*>::Insert in order to modify
    insertion code to sort effects lexicographically instead of by
    type only.  The ordering by ID, subtype (if any), creator, then
    spell id.
  * Add memory edit so that all effects default to an array of 10
    integers.
  * Remove all core2.8 API and return to NWNX Exalt api.  I was unable
    to get the core2.8 functional in the face of any compiler
    optimizations.		
		
* v0.3
  * Hook CNWSCreature::RemoveBadEffects.
      * This function controls only what effects are removed on rest,
        force rest, DM rest. Totally Replace original engine code
        which had some weird effect type cap at
        EFFECT\_TRUETYPE\_WOUNDING.  Thus it was ignoring any effect
        types beyond that.
* v0.2
    * Removed special custom effect hooks, NWScript functions.  This has
      been obsoleted by the hooks for effect application and removal.
	* Updated to (my current fork of) NWNX 2.8 api 
	* New Hooks 
	    * CServerAIMaster__OnEffectApplied
	    * CServerAIMaster__OnEffectRemoved
	    * CServerAIMaster__OnItemPropertyApplied
	    * CServerAIMaster__OnItemPropertyRemoved
	    * ExecuteCommandApplyEffectOnObject
