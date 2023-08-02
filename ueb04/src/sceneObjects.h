

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

void drawPlane(const CGColor3f color);

void drawSphere(const CGColor3f color);
void drawNormalPlane(const CGColor3f color);