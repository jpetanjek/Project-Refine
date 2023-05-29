class PR_ActiveMapIconFobClass : PR_ActiveMapIconClass
{
}

class PR_ActiveMapIconFob : PR_ActiveMapIcon
{
	override protected void UpdatePosAndDirPropFromTarget()
	{
		// Same as base class, but we don't copy direction
		vector worldTransform[4];
		m_Target.GetWorldTransform(worldTransform);
		m_vPosAndDir = Vector(worldTransform[3][0], 0, worldTransform[3][2]); // X, Dir, Z
	}
}