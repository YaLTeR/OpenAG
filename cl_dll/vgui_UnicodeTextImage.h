#ifndef VGUI_UNICODE_TEXT_IMAGE_H
#define VGUI_UNICODE_TEXT_IMAGE_H
#include <string>
#include <VGUI_TextImage.h>

/**
 * A VGUI1 text image that supports Unicode by rendering chars via VGUI2.
 * It can only draw text in one line.
 * If VGUI2 is not available, it falls back to VGUI1 TextImage behavior.
 */
class UnicodeTextImage : public vgui::TextImage
{
public:
	using BaseClass = vgui::TextImage;

	/**
	 * Type for VGUI2 font handle.
	 */
	using HFont = unsigned int;

	/**
	 * An invalid VGUI2 font handle.
	 */
	static constexpr HFont INVALID_FONT = 0;

	/**
	 * Minimum size of font to enable antialiasing in display pixels.
	 * Samll fonts with antialiasing look blurry.
	 */
	static constexpr int MIN_AA_FONT_SIZE = 21;

	/**
	 * Loads interfaces from the engine.
	 */
	static void initInterfaces();

	/**
	 * Returns true if UnicodeTextImage falls back to vgui::TextImage.
	 * If true, it will behave exactly like TextImage, without unicode support.
	 */
	static bool shouldFallback();

	/**
	 * Creates a font.
	 * @param	fontName	Name of the font in the OS
	 * @param	tall		Height of the font in pixels
	 * @param	weight		Weight of the font
	 * @param	antialias	Enable antialiasing (if tall >= MIN_AA_FONT_SIZE or vgui_unicode_aa = 2)
	 */
	static HFont createFont(const char *fontName, int tall, int weight);

	//------------------------------------------------------------------

	/**
	 * Contstructs an empty TextImage.
	 */
	UnicodeTextImage();

	/**
	 * Returns size of the text.
	 */
	virtual void getTextSize(int &wide, int &tall) override;

	/**
	 * Unsupported.
	 */
	virtual void getTextSizeWrapped(int &wide, int &tall) override;

	/**
	 * Returns VGUI2 font handle.
	 */
	virtual HFont getUnicodeFont();

	/**
	 * Sets image text.
	 * @param	textBufferLen	Size of text buffer. Must be at least strlen(text) + 1
	 * @param	text			UTF-8 encoded text
	 */
	virtual void setText(int textBufferLen, const char *text) override;

	/**
	 * Sets image text.
	 * @param	text	UTF-8 encoded text
	 */
	virtual void setText(const char *text) override;
	
	/**
	 * Sets the font of the text.
	 * @param	font			Unicode VGUI2 font
	 * @param	fallbackFont	VGUI1 font if Unicode is disabled/not available
	 */
	virtual void setFont(HFont font, vgui::Font *fallbackFont);

	/**
	 * Unsupported.
	 */
	virtual void setFont(vgui::Scheme::SchemeFont schemeFont) override;

	/**
	 * Unsupported.
	 */
	virtual void setFont(vgui::Font *font) override;

	virtual void setPos(int x, int y) override;

protected:
	virtual void paint(vgui::Panel *panel) override;

private:
	HFont m_Font = INVALID_FONT;
	std::wstring m_Text;
};

#endif
