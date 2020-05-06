Le JVSy est une solution opensource à bas coût  pour interfacer un PC avec une borne d'arcade au standard JVS*. Le PC détecte les contrôles de la borne en tant que contrôleurs de jeu USB Direct Input.

La solution s'appuie sur une carte de développement Teensy V2 et une puce SN65176B permettant la communication sur le protocole RS485 utilisé par le bus JVS. Une PCB additionnelle a été désignée dans le but de simplifier le raccordement des composants et d'intégrer un dipswitch pour la sélection facile des différents mode et options supportés.

En l'état actuel du projet, le JVSy est une solution à bas coût aux produits du marché tels que le JVS-PAC ou le JVS-Strike avec l'avantage d'offrir un support pour les guncabs. Il est donc possible de jouer en émulation à des jeux prévus pour des pistolets optiques comme Point Blank ou des pistolets positionnels comme Terminator 2 sur une borne de tir Naomi Universal comme Confidential Mission par exemple.

Le développement du JVSy est actif et en cours et l'ajout des fonctionnalités suivantes est en développement :
- le chaînage d'I/O boards : pour linker des bornes ensemble et avoir une configuration Versus Fighting où chaque joueur joue sur une borne ou bien pour permettre le jeu simultané à 4 joueurs répartis sur 2 bornes.
- le support des trackballs : le chaînage d'I/O permettra également le support des panels spécifiques à certains jeux comme Outriggers ou Virtua Golf utilisant un trackball sur une mini I/O board secondaire intégrée au panel.
- le support des sticks analogiques tels que ceux utilisés sur les panels des jeux Virtua Strikers ou des jeux plus récents des bornes HD comme Final Fantasy Dissidia.



(*)voir liste des bornes compatibles

Bornes compatibles (liste non-exhaustive) :
Sega Naomi Universal Cabinet (NUC)
Sega New Net City
Namco Noir HD¹
Konami Windy 2

Compatibilité à évaluer :
Taito Vewlix (I/O standard, pas de Fast I/O)
Taito Egret 3
Sega Blast City
Sega Lindbergh
Namco Cyberlead
Sammy Atomiswave

(¹) Option à activer

Merci à k4roshi pour son travail dont est issu ce projet.


JVSY is a lowcost opensource solution to interface a PC with an arcade cabinet compliant with the JVS standard*. The computer detects cabinet controls as USB Direct Input game controllers.

This solution is based on a Teensy V2 development board and a SN65176B chip allowing communication over the RS485 protocol used by the JVS bus. An additional PCB was designed to simplify the connection between components and to integrate a dipswitch to easily select the different supported modes and options.
Currently, JVSy is a lowcost alternative to regular market solutions like JVS-PAC or JVS-Strike with the benefit of the guncabs support. Thus it is possible to play with emulators to games designed for optical guns like Point Blank or games designed for positional guns like Terminator 2 on a Naomi Universal guncab like Confidential Mission for instance.

JVSy development is active and ongoing. The following features are under development :

- I/O boards link : will allow to link arcade cabinets together and allow 4 players simultanous play over 2 separate cabinets.

- Trackball support : chaining I/O boards will also allow to support control panels that are specific to games like Outriggers or Virtual Golf using a trackball plugged on a secondary mini I/O board integrated to the panel.

- Analog sticks support as the ones found on control panels for games like Virtual Strikers or some more recent games on HD cabinets like Final Fantasy Dissidia.

(*)see list of confirmed compatible cabinets
Confirmed compatible cabinets :
Sega Naomi Universal Cabinet (NUC), Sega New Net City, Namco Noir HD¹, Konami Windy 2
Compatibility to be confirmed :
Taito Vewlix (standard JVS I/O only , no Fast I/O), Taito Egret 3, Sega Blast City, Sega Lindbergh, Namco Cyberlead, Sammy Atomiswave...
(¹) Option to activate

Many thanks to k4roshi for his work which was the base for this project.

