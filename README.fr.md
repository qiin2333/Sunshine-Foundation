# Sunshine Foundation Edition

## 🌐 Support multilingue / Multi-language Support

<div align="center">

[![English](https://img.shields.io/badge/English-README.en.md-blue?style=for-the-badge)](README.en.md)
[![简体中文](https://img.shields.io/badge/简体中文-README.zh--CN.md-red?style=for-the-badge)](README.md)
[![Français](https://img.shields.io/badge/Français-README.fr.md-green?style=for-the-badge)](README.fr.md)
[![Deutsch](https://img.shields.io/badge/Deutsch-README.de.md-yellow?style=for-the-badge)](README.de.md)
[![日本語](https://img.shields.io/badge/日本語-README.ja.md-purple?style=for-the-badge)](README.ja.md)

</div>

---

Fork basé sur LizardByte/Sunshine, offrant une documentation complète [Lire la documentation](https://docs.qq.com/aio/DSGdQc3htbFJjSFdO?p=YTpMj5JNNdB5hEKJhhqlSB).

**Sunshine-Foundation** est un hôte de streaming de jeu auto-hébergé pour Moonlight. Cette version forkée apporte des améliorations significatives par rapport à Sunshine original, en se concentrant sur l'amélioration de l'expérience de streaming de jeu entre divers appareils terminaux et l'hôte Windows :

### 🌟 Fonctionnalités principales
- **Support convivial HDR** - Pipeline de traitement HDR optimisé pour une véritable expérience de streaming de jeux HDR
- **Écran virtuel** - Gestion intégrée des écrans virtuels, permettant de créer et gérer des écrans virtuels sans logiciel supplémentaire
- **Microphone distant** - Prise en charge de la réception du microphone client, offrant une transmission vocale de haute qualité
- **Panneau de contrôle avancé** - Interface Web intuitive de contrôle, fournissant une surveillance en temps réel et une gestion de configuration
- **Transmission à faible latence** - Traitement de codage optimisé exploitant les dernières capacités matérielles
- **Appairage intelligent** - Gestion intelligente des profils correspondants aux appareils appairés

### 🖥️ Intégration d'écran virtuel (Nécessite Win10 22H2 ou système plus récent)
- Création et destruction dynamique d'écrans virtuels
- Prise en charge des résolutions et taux de rafraîchissement personnalisés
- Gestion de configuration multi-écrans
- Modifications de configuration en temps réel sans redémarrage

## Clients Moonlight recommandés

Il est recommandé d'utiliser les clients Moonlight suivants optimisés pour une expérience de streaming optimale (activation des propriétés du set) :

### 🖥️ Clients Windows(X86_64, Arm64), MacOS, Linux
[![Moonlight-PC](https://img.shields.io/badge/Moonlight-PC-red?style=for-the-badge&logo=windows)](https://github.com/qiin2333/moonlight-qt)

### 📱 Client Android
[![Édition renforcée Moonlight-Android](https://img.shields.io/badge/Édition_renforcée-Moonlight--Android-green?style=for-the-badge&logo=android)](https://github.com/qiin2333/moonlight-android/releases/tag/shortcut)
[![Édition Crown Moonlight-Android](https://img.shields.io/badge/Édition_Crown-Moonlight--Android-blue?style=for-the-badge&logo=android)](https://github.com/WACrown/moonlight-android)

### 📱 Client iOS
[![Terminal Void Moonlight-iOS](https://img.shields.io/badge/Voidlink-Moonlight--iOS-lightgrey?style=for-the-badge&logo=apple)](https://github.com/The-Fried-Fish/VoidLink)

### 🛠️ Autres ressources
[awesome-sunshine](https://github.com/LizardByte/awesome-sunshine)

## Configuration système requise

> [!WARNING]
> Ces tableaux sont continuellement mis à jour. Veuillez ne pas acheter de matériel uniquement sur la base de ces informations.

<table>
    <caption id="minimum_requirements">Configuration minimale requise</caption>
    <tr>
        <th>Composant</th>
        <th>Exigence</th>
    </tr>
    <tr>
        <td rowspan="3">GPU</td>
        <td>AMD : VCE 1.0 ou version ultérieure, voir : <a href="https://github.com/obsproject/obs-amd-encoder/wiki/Hardware-Support">obs-amd support matériel</a></td>
    </tr>
    <tr>
        <td>Intel : Compatible VAAPI, voir : <a href="https://www.intel.com/content/www/us/en/developer/articles/technical/linuxmedia-vaapi.html">Support matériel VAAPI</a></td>
    </tr>
    <tr>
        <td>Nvidia : Carte graphique supportant NVENC, voir : <a href="https://developer.nvidia.com/video-encode-and-decode-gpu-support-matrix-new">Matrice de support nvenc</a></td>
    </tr>
    <tr>
        <td rowspan="2">CPU</td>
        <td>AMD : Ryzen 3 ou supérieur</td>
    </tr>
    <tr>
        <td>Intel : Core i3 ou supérieur</td>
    </tr>
    <tr>
        <td>RAM</td>
        <td>4GB ou plus</td>
    </tr>
    <tr>
        <td rowspan="5">Système d'exploitation</td>
        <td>Windows : 10 22H2+ (Windows Server ne supporte pas les manettes de jeu virtuelles)</td>
    </tr>
    <tr>
        <td>macOS : 12+</td>
    </tr>
    <tr>
        <td>Linux/Debian : 12+ (bookworm)</td>
    </tr>
    <tr>
        <td>Linux/Fedora : 39+</td>
    </tr>
    <tr>
        <td>Linux/Ubuntu : 22.04+ (jammy)</td>
    </tr>
    <tr>
        <td rowspan="2">Réseau</td>
        <td>Hôte : 5GHz, 802.11ac</td>
    </tr>
    <tr>
        <td>Client : 5GHz, 802.11ac</td>
    </tr>
</table>

<table>
    <caption id="4k_suggestions">Configuration recommandée pour la 4K</caption>
    <tr>
        <th>Composant</th>
        <th>Exigence</th>
    </tr>
    <tr>
        <td rowspan="3">GPU</td>
        <td>AMD : Video Coding Engine 3.1 ou supérieur</td>
    </tr>
    <tr>
        <td>Intel : HD Graphics 510 ou supérieur</td>
    </tr>
    <tr>
        <td>Nvidia : GeForce GTX 1080 ou modèle supérieur avec encodeurs multiples</td>
    </tr>
    <tr>
        <td rowspan="2">CPU</td>
        <td>AMD : Ryzen 5 ou supérieur</td>
    </tr>
    <tr>
        <td>Intel : Core i5 ou supérieur</td>
    </tr>
    <tr>
        <td rowspan="2">Réseau</td>
        <td>Hôte : Ethernet CAT5e ou supérieur</td>
    </tr>
    <tr>
        <td>Client : Ethernet CAT5e ou supérieur</td>
    </tr>
</table>

## Support technique

Procédure de résolution des problèmes :
1. Consultez la [documentation d'utilisation](https://docs.qq.com/aio/DSGdQc3htbFJjSFdO?p=YTpMj5JNNdB5hEKJhhqlSB) [Documentation LizardByte](https://docs.lizardbyte.dev/projects/sunshine/latest/)
2. Activez le niveau de journalisation détaillé dans les paramètres pour trouver des informations pertinentes
3. [Rejoignez le groupe QQ pour obtenir de l'aide](https://qm.qq.com/cgi-bin/qm/qr?k=5qnkzSaLIrIaU4FvumftZH_6Hg7fUuLD&jump_from=webapi)
4. [Utilisez deux lettres !](https://uuyc.163.com/)

**Étiquettes de signalement des problèmes :**
- `hdr-support` - Problèmes liés au HDR
- `virtual-display` - Problèmes d'écran virtuel
- `config-help` - Problèmes liés à la configuration

## 📚 Documentation de développement

- **[Instructions de compilation](docs/building.md)** - Instructions pour compiler et construire le projet
- **[Guide de configuration](docs/configuration.md)** - Description des options de configuration d'exécution
- **[Développement WebUI](docs/WEBUI_DEVELOPMENT.md)** - Guide complet du développement de l'interface Web Vue 3 + Vite

## Rejoignez la communauté

Nous accueillons favorablement les discussions et les contributions de code !
[![Rejoindre le groupe QQ](https://pub.idqqimg.com/wpa/images/group.png 'Rejoindre le groupe QQ')](https://qm.qq.com/cgi-bin/qm/qr?k=WC2PSZ3Q6Hk6j8U_DG9S7522GPtItk0m&jump_from=webapi&authKey=zVDLFrS83s/0Xg3hMbkMeAqI7xoHXaM3sxZIF/u9JW7qO/D8xd0npytVBC2lOS+z)

## Historique des stars

[![Graphique d'historique des stars](https://api.star-history.com/svg?repos=qiin2333/Sunshine-Foundation&type=Date)](https://www.star-history.com/#qiin2333/Sunshine-Foundation&Date)

---

**Sunshine Foundation Edition - Rendre le streaming de jeux plus élégant**
```