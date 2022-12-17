modded class SCR_ChimeraCharacter : ChimeraCharacter
{
	protected static ref array<SCR_ChimeraCharacter> s_aAll = {};
	
	SCR_CharacterDamageManagerComponent m_DamageManager;
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		m_pFactionComponent = FactionAffiliationComponent.Cast(FindComponent(FactionAffiliationComponent));
		m_DamageManager = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));
		
		s_aAll.Insert(this);
	}
	
	// Returns array with all characters
	// Clears up the values so that no nulls are returned
	static array<SCR_ChimeraCharacter> GetAllCharacters()
	{
		for (int i = s_aAll.Count() - 1; i >= 0; i--)
		{
			if (!s_aAll[i])
				s_aAll.Remove(i);
		}
		array<SCR_ChimeraCharacter> a = {};
		a.Copy(s_aAll);
		return a;
	}
};