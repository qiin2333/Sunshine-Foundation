# Édition de base Sunshine

Basé sur la succursale LizardByte / Sunshine, une prise en charge complète de la documentation est fournie [Lire les documents] (https://docs.qq.com/aio/dsgdqc3htbfjjsfdo?p=ytpmj5jnndb5hekjhhqlsb).

** Sunshine-Foundation ** est un hôte de flux de jeu auto-hébergé pour Moonlight. Cette version de branche a apporté des améliorations significatives en fonction du soleil d'origine, en se concentrant sur l'amélioration de l'expérience de streaming de jeux de divers appareils de terminaux de streaming et hôtes Windows:

### 🌟 fonctionnalités de base
- ** Support HDR-Friendly ** - Pipe de traitement HDR optimisé pour une véritable expérience de streaming de jeu HDR
- ** Affichage virtuel intégré ** - Gestion de l'affichage virtuel intégré, création et gestion des moniteurs virtuels sans logiciel supplémentaire
- ** Microphone distant ** - prend en charge la réception des microphones clients, offrant une fonction de passage vocale de haute qualité
- ** Panneau de contrôle avancé ** - Interface de contrôle Web intuitive, fournissant une surveillance et une gestion de la configuration en temps réel
- ** Transmission à faible latence ** - Traitement de codage optimisé avec les dernières capacités matérielles
- ** Association intelligente ** - Gérez intelligemment les fichiers de configuration correspondants des périphériques d'appariement

### 🖥️ L'intégration du moniteur virtuel (nécessite des systèmes Win10 22H2 et plus récents)
- Création et destruction de l'affichage virtuel dynamique
- Résolution personnalisée et prise en charge du taux de rafraîchissement
- Gestion de la configuration multi-affectation
- Modifications de configuration en temps réel sans redémarrage


## Client de Moonlight recommandé

Il est recommandé d'utiliser le client Moonlight optimisé suivant pour la meilleure expérience de streaming (activer les propriétés de l'ensemble):

### 🖥️ Windows (x86_64, ARM64), macOS, client Linux
[! [Moonlight-pc] (https://img.shields.io/badge/moonlight-pc-red?style=for-the-badge&logo=windows)] (https://github.com/qiin2333/moonlight-qt)

### 📱 Client Android
[! [Weil Rehanced Version Moonlight-Android] (https://img.shields.io/badge/weil Enhanced Version-Moonlight - Android-Green? Style = For-The Badge & Logo = Android)] (https://github.com/qiin2333/moonlight-android/releases/tag/shortcut)
[![Version de la Couronne Moonlight-Android] (https://img.shields.io/badge/crown version-moonlight-android-blue? Style = for-the-badge & logo = Android)] (https://github.com/wacrown/moonlight -ndroid)

### 📱 Client iOS
[![Real Brick Edition Moonlight-IOS] (https://img.shields.io/badge/real Brick Edition-Moonlight --ios-lightgrey? Style = For-TheBadge & Logo = Apple)] (https://github.com/truezhuangjia/moonlight-ios- nativemultichpassthrough)


### 🛠️ Autres ressources
[Awesome-Sunshine] (https://github.com/lizardbyte/awesome-sunshine)

## Exigences du système


> [! Avertissement]
> Ces tables sont mises à jour en continu. Veuillez ne pas acheter de matériel basé uniquement sur ces informations.


<ballage>
<légende id = "minimum_requirements"> Exigences de configuration minimale </pention>
<tr>
<th> Composant </th>
<th> exigences </th>
</tr>
<tr>
<td rowspan = "3"> gpu </td>
<TD> AMD: VCE 1.0 ou version ultérieure, voir: <a href = "https://github.com/obsproject/obs-amd-encoder/wiki/hardware-support"> Obs -amd matériel support </a> </ td>
</tr>
<tr>
<td> Intel: Vaapi Compatible, voir: <a href = "https://www.intel.com/content/www/us/en/developer/articles/technical/linuxmedia-vaapi.html"> support matériel VAAPI </a> </td>
</tr>
<tr>
<TD> Nvidia: Cartes graphiques soutenues par NVenc, voir: <a href = "https://developer.nvidia.com/video-encode-and-decode-gpu-support-matrix-new"> nvence support matrix </a> </td>
</tr>
<tr>
<td Rowspan = "2"> CPU </TD>
<TD> AMD: Ryzen 3 ou plus </td>
</tr>
<tr>
<TD> Intel: Core i3 ou supérieur </td>
</tr>
<tr>
<TD> RAM </TD>
<TD> 4 Go ou plus </td>
</tr>
<tr>
<td Rowspan = "5"> Système d'exploitation </td>
<TD> Windows: 10 22H2 + (Windows Server ne prend pas en charge le jeu de jeu virtuel) </td>
</tr>
<tr>
<td> macOS: 12 + </td>
</tr>
<tr>
<Td> Linux / Debian: 12+ (Bookworm) </td>
</tr>
<tr>
<TD> Linux / Fedora: 39 + </td>
</tr>
<tr>
<Td> Linux / Ubuntu: 22.04+ (Jammy) </td>
</tr>
<tr>
<td Rowspan = "2"> réseau </td>
<TD> Hôte: 5GHz, 802.11ac </td>
</tr>
<tr>
<TD> Client: 5GHz, 802.11ac </td>
</tr>
</ table>

<ballage>
<légende id = "4k_suggestions"> 4K Configuration recommandée </pention>
<tr>
<th> Composant </th>
<th> exigences </th>
</tr>
<tr>
<td rowspan = "3"> gpu </td>
<TD> AMD: moteur de codage vidéo 3.1 ou plus </td>
</tr>
<tr>
<TD> Intel: HD Graphics 510 ou plus </td>
</tr>
<tr>
<TD> NVIDIA: GEFORCE GTX 1080 ou plus modèles avec multi-encodeur </td>
</tr>
<tr>
<td Rowspan = "2"> CPU </TD>
<TD> AMD: Ryzen 5 ou plus </td>
</tr>
<tr>
<TD> Intel: Core i5 ou supérieur </td>
</tr>
<tr>
<td Rowspan = "2"> réseau </td>
<TD> Hôte: Cat5e Ethernet ou mieux </td>
</tr>
<tr>
<TD> Client: Cat5e Ethernet ou mieux </td>
</tr>
</ table>

## Assistance technique

Chemin de solution lors de la rencontre des problèmes:
1. Voir [Document utilisateur] (https://docs.qq.com/aio/dsgdqc3htbfjjsfdo?p=ytpmj5jnndb5hekjhhqlsb) [Document LizardByte] (https://docs.lizardbyte.dev/projects/sunshine/latest/)
2. Ouvrez le niveau de journal détaillé dans les paramètres pour trouver des informations pertinentes
3. [Rejoignez le groupe de communication QQ pour obtenir de l'aide] (https://qm.qq.com/cgi-bin/qm/qr?k=5qnkzsaliriau4fvumftzh_6hg7fuuld&jump_from=webapi)
4. [Utilisez deux lettres!] (https://uuyc.163.com/)

** Tag de rétroaction du problème: **
- «HDR-Support» - Problèmes liés à HDR
- `Virtual-Display` - Problème d'affichage virtuel
- `config-help` - problèmes liés à la configuration

## Rejoignez la communauté

Nous accueillons tout le monde à participer à la discussion et à contribuer au code!
[! [Rejoignez le groupe QQ] (https://pub.idqqimg.com/wpa/images/group.png 'join Groupe ')] (https://qm.qq.com/cgi-bin/qm/qr?k=WC2PSZ3Q6HK6J8U_DG9S7522GPTITK0M&jump_fr om = webapi & authkey = zvdlfrs83s / 0xg3hmbkmeaqi7xohxam3sxzif / u9jw7qo / d8xd0npytvbc2los + z)

## HISTOIRE DE STAR

[! [Star History Chart] (https://api.star-history.com/svg?repos=qiin2333/sunshine-foundation&type=date)] (https://www.star-history.com/#qiin2333/sunshine-foundation&date)

---

** Édition de base Sunshine - Rendre le streaming du jeu plus facile **