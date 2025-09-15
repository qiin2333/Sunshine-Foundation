# Sunshine Foundation Edition

## 🌐 Mehrsprachige Unterstützung / Multi-language Support

<div align="center">

[![English](https://img.shields.io/badge/English-README.en.md-blue?style=for-the-badge)](README.en.md)
[![中文简体](https://img.shields.io/badge/中文简体-README.zh--CN.md-red?style=for-the-badge)](README.md)
[![Français](https://img.shields.io/badge/Français-README.fr.md-green?style=for-the-badge)](README.fr.md)
[![Deutsch](https://img.shields.io/badge/Deutsch-README.de.md-yellow?style=for-the-badge)](README.de.md)
[![日本語](https://img.shields.io/badge/日本語-README.ja.md-purple?style=for-the-badge)](README.ja.md)

</div>

---

Ein Fork basierend auf LizardByte/Sunshine, bietet vollständige Dokumentationsunterstützung [Read the Docs](https://docs.qq.com/aio/DSGdQc3htbFJjSFdO?p=YTpMj5JNNdB5hEKJhhqlSB).

**Sunshine-Foundation** ist ein selbst gehosteter Game-Stream-Host für Moonlight. Diese Fork-Version hat erhebliche Verbesserungen gegenüber dem ursprünglichen Sunshine vorgenommen und konzentriert sich darauf, das Spielestreaming-Erlebnis für verschiedene Streaming-Endgeräte und deren Verbindung zum Windows-Host zu verbessern:

### 🌟 Kernfunktionen
- **HDR-freundliche Unterstützung** - Optimierte HDR-Verarbeitungspipeline für ein echtes HDR-Game-Streaming-Erlebnis
- **Virtuelle Anzeige** - Integriertes Management virtueller Anzeigen, ermöglicht das Erstellen und Verwalten ohne zusätzliche Software
- **Entferntes Mikrofon** - Unterstützt das Empfangen von Client-Mikrofonen mit hochwertiger Sprachdurchreichung
- **Erweiterte Systemsteuerung** - Intuitive Web-Oberfläche zur Konfiguration mit Echtzeit-Überwachung
- **Niedrige Latenzübertragung** - Optimierte Encoder-Verarbeitung unter Nutzung der neuesten Hardware-Fähigkeiten
- **Intelligente Paarung** - Intelligentes Management von Profilen für gepaarte Geräte

### 🖥️ Integrierte virtuelle Anzeige (Benötigt win10 22H2 oder neuer)
- Dynamisches Erstellen und Entfernen virtueller Anzeigen
- Unterstützung für benutzerdefinierte Auflösungen und Bildwiederholraten
- Verwaltung von Mehrfachanzeige-Konfigurationen
- Echtzeit-Konfigurationsänderungen ohne Neustart


## Empfohlene Moonlight-Clients

Für das beste Streaming-Erlebnis wird die Verwendung der folgenden optimierten Moonlight-Clients empfohlen (Aktiviert Set-Boni):

### 🖥️ Windows(X86_64, Arm64), MacOS, Linux Client
[![Moonlight-PC](https://img.shields.io/badge/Moonlight-PC-red?style=for-the-badge&logo=windows)](https://github.com/qiin2333/moonlight-qt)

### 📱 Android Client
[![Verstärkte Edition Moonlight-Android](https://img.shields.io/badge/Verstärkte_Edition-Moonlight--Android-green?style=for-the-badge&logo=android)](https://github.com/qiin2333/moonlight-android/releases/tag/shortcut)
[![Kronen Edition Moonlight-Android](https://img.shields.io/badge/Kronen_Edition-Moonlight--Android-blue?style=for-the-badge&logo=android)](https://github.com/WACrown/moonlight-android)

### 📱 iOS Client
[![Voidlink Moonlight-iOS](https://img.shields.io/badge/Voidlink-Moonlight--iOS-lightgrey?style=for-the-badge&logo=apple)](https://github.com/The-Fried-Fish/VoidLink)


### 🛠️ Weitere Ressourcen
[awesome-sunshine](https://github.com/LizardByte/awesome-sunshine)

## Systemanforderungen


> [!WARNING]
> Diese Tabellen werden kontinuierlich aktualisiert. Bitte kaufen Sie Hardware nicht allein basierend auf diesen Informationen.


<table>
    <caption id="minimum_requirements">Mindestanforderungen</caption>
    <tr>
        <th>Komponente</th>
        <th>Anforderung</th>
    </tr>
    <tr>
        <td rowspan="3">GPU</td>
        <td>AMD: VCE 1.0 oder höher, siehe: <a href="https://github.com/obsproject/obs-amd-encoder/wiki/Hardware-Support">obs-amd Hardware-Unterstützung</a></td>
    </tr>
    <tr>
        <td>Intel: VAAPI-kompatibel, siehe: <a href="https://www.intel.com/content/www/us/en/developer/articles/technical/linuxmedia-vaapi.html">VAAPI Hardware-Unterstützung</a></td>
    </tr>
    <tr>
        <td>Nvidia: Grafikkarte mit NVENC-Unterstützung, siehe: <a href="https://developer.nvidia.com/video-encode-and-decode-gpu-support-matrix-new">NVENC-Unterstützungsmatrix</a></td>
    </tr>
    <tr>
        <td rowspan="2">CPU</td>
        <td>AMD: Ryzen 3 oder höher</td>
    </tr>
    <tr>
        <td>Intel: Core i3 oder höher</td>
    </tr>
    <tr>
        <td>RAM</td>
        <td>4GB oder mehr</td>
    </tr>
    <tr>
        <td rowspan="5">Betriebssystem</td>
        <td>Windows: 10 22H2+ (Virtuelle Gamepads werden auf Windows Server nicht unterstützt)</td>
    </tr>
    <tr>
        <td>macOS: 12+</td>
    </tr>
    <tr>
        <td>Linux/Debian: 12+ (bookworm)</td>
    </tr>
    <tr>
        <td>Linux/Fedora: 39+</td>
    </tr>
    <tr>
        <td>Linux/Ubuntu: 22.04+ (jammy)</td>
    </tr>
    <tr>
        <td rowspan="2">Netzwerk</td>
        <td>Host: 5GHz, 802.11ac</td>
    </tr>
    <tr>
        <td>Client: 5GHz, 802.11ac</td>
    </tr>
</table>

<table>
    <caption id="4k_suggestions">Empfohlene Konfiguration für 4K</caption>
    <tr>
        <th>Komponente</th>
        <th>Anforderung</th>
    </tr>
    <tr>
        <td rowspan="3">GPU</td>
        <td>AMD: Video Coding Engine 3.1 oder höher</td>
    </tr>
    <tr>
        <td>Intel: HD Graphics 510 oder höher</td>
    </tr>
    <tr>
        <td>Nvidia: GeForce GTX 1080 oder höhere Modelle mit mehreren Encodern</td>
    </tr>
    <tr>
        <td rowspan="2">CPU</td>
        <td>AMD: Ryzen 5 oder höher</td>
    </tr>
    <tr>
        <td>Intel: Core i5 oder höher</td>
    </tr>
    <tr>
        <td rowspan="2">Netzwerk</td>
        <td>Host: CAT5e Ethernet oder besser</td>
    </tr>
    <tr>
        <td>Client: CAT5e Ethernet oder besser</td>
    </tr>
</table>

## Technischer Support

Lösungsweg bei Problemen:
1. Konsultieren Sie die [Nutzungsdokumentation](https://docs.qq.com/aio/DSGdQc3htbFJjSFdO?p=YTpMj5JNNdB5hEKJhhqlSB) [LizardByte-Dokumentation](https://docs.lizardbyte.dev/projects/sunshine/latest/)
2. Aktivieren Sie den detaillierten Log-Level in den Einstellungen, um relevante Informationen zu finden
3. [Treten Sie der QQ-Gruppe bei, um Hilfe zu erhalten](https://qm.qq.com/cgi-bin/qm/qr?k=5qnkzSaLIrIaU4FvumftZH_6Hg7fUuLD&jump_from=webapi)
4. [Benutze zwei Buchstaben!](https://uuyc.163.com/)

**Problemrückmeldung-Labels:**
- `hdr-support` - Probleme im Zusammenhang mit HDR
- `virtual-display` - Probleme mit virtuellen Anzeigen
- `config-help` - Probleme im Zusammenhang mit der Konfiguration

## 📚 Entwicklerdokumentation

- **[Build-Anleitung](docs/building.md)** - Anleitung zum Kompilieren und Erstellen des Projekts
- **[Konfigurationshandbuch](docs/configuration.md)** - Erläuterung der Laufzeit-Konfigurationsoptionen
- **[WebUI-Entwicklung](docs/WEBUI_DEVELOPMENT.md)** - Vollständige Anleitung zur Entwicklung der Vue 3 + Vite Web-Oberfläche

## Community beitreten

Wir heißen Diskussionen und Code-Beiträge willkommen!
[![QQ-Gruppe beitreten](https://pub.idqqimg.com/wpa/images/group.png 'QQ-Gruppe beitreten')](https://qm.qq.com/cgi-bin/qm/qr?k=WC2PSZ3Q6Hk6j8U_DG9S7522GPtItk0m&jump_from=webapi&authKey=zVDLFrS83s/0Xg3hMbkMeAqI7xoHXaM3sxZIF/u9JW7qO/D8xd0npytVBC2lOS+z)

## Star-Historie

[![Star-Historie-Diagramm](https://api.star-history.com/svg?repos=qiin2333/Sunshine-Foundation&type=Date)](https://www.star-history.com/#qiin2333/Sunshine-Foundation&Date)

---

**Sunshine Foundation Edition - Macht Game-Streaming eleganter**