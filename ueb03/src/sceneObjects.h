

/**
 * liefert, ob die Normalen angezeigt werden sollen
 * @return true, wenn die Normalen angezeigt werden sollen, sonst false
 */
GLboolean getShowNormal(void);

/**
 * Setzt das Anzeigen der Normalen
 * @param normal true, wenn die Normalen angezeigt werden sollen, sonst false
 */ 
void setShowNormal(GLboolean normal);


/**
 * Zeichnet eine Kugel in der uebergebenen Farbe
 * @param color Farbe der Kugel
 */
void drawSphere(const CGColor3f color);

/**
 * Zeichnet alle Baelle
 */
void drawBalls();

/**
 * Zeichnet eine transparente Kugel in der uebergebenen Farbe
 * @param color Farbe der Kugel
 */
void drawTransparentSphere(const CGColor4f color);

/**
 * Zeichnet alle schwarzen Loecher
 */
void drawBlackHoles();
