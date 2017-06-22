//---------------------------------------------------------------------------------------------------------------------------
//INLINE
//---------------------------------------------------------------------------------------------------------------------------
inline Entity::Entity()
	: m_position(IntVector2(-1, -1))
	, m_glyph(' ')
	, m_color(RGBA::WHITE)
	, m_backgroundColor(RGBA::WHITE)
	, m_name("")
	, m_id(s_entityIDs++)
	, m_belongingMap(nullptr)
	, m_squirrelFont(nullptr)
{ 
	m_squirrelFont = BitmapFont::CreateOrGetFont("SquirrelFont");
}

inline Entity::Entity(const IntVector2& position, char glyph, const RGBA& color, const RGBA& bgColor, const String& name, Map* const map)
	: m_position(position)
	, m_glyph(glyph)
	, m_color(color)
	, m_backgroundColor(bgColor)
	, m_name(name)
	, m_id(s_entityIDs++)
	, m_belongingMap(map)
	, m_squirrelFont(nullptr)
{
	m_squirrelFont = BitmapFont::CreateOrGetFont("SquirrelFont");
}