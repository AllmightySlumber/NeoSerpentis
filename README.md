# Snake

Pour faire tourner ce projet, il faut être en possession de deux matrices leds 64x64, deux arduino, d'un joystick connecter à l'un des deux arduino qui sera donc l'arduino principale.

Branchement :
PC -> Arduino principale -> Matrice principale 
Arduino principale -> joystick
PC -> Arduino secondaire -> Matrice secondaire
Arduino principale -> Arduino secondaire

Chargement des programmes :
Snake -> Arduino principale
ScoreBoard -> Arduino secondaire

Principe de communication :
L'arduino principale reçoit les signaux du joystick et les répercute soit sur la matrice principale en cas de partie en cours soit sur l'arduino secondaire qui repercute sur la matrice secondaire pour naviguer dans le menu afficher sur cette matrice.

Spécificités:
Serpent qui traverse les murs et ressort de l'autre côté
Nourriture de couleurs avec différents effets
Fonction de création et de suppression de nourriture avec un délai aléatoire compris dans un certain intervalle.
