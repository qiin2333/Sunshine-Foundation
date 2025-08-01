＃サンシャインベースエディション

Lizardbyte/Sunshine Branchに基づいて、完全なドキュメントサポートが提供されます[docsを読む]（https://docs.qq.com/aio/dsgdqc3htbfjjsfdo?p=ytpmj5jnndb5hekjhhqlsb）。

** Sunshine-Foundation **は、Moonlightの自己ホストのゲームストリームホストです。このブランチバージョンは、元のサンシャインに基づいて大幅に改善され、さまざまなストリーミング端末デバイスとWindowsホストのゲームストリーミングエクスペリエンスの改善に焦点を当てています。

### coreコア機能
-  ** HDRフレンドリーサポート**  - 真のHDRゲームストリーミングエクスペリエンスのための最適化HDR処理パイプライン
-  **統合された仮想ディスプレイ**  - ビルトイン仮想ディスプレイ管理、追加のソフトウェアなしで仮想モニターの作成と管理
-  **リモートマイク**  - クライアントマイクの受信をサポートし、高品質の音声パススルー機能を提供します
-  **高度なコントロールパネル**  - リアルタイムの監視と構成管理を提供する直感的なWebコントロールインターフェイス
-  **低レイテンシ伝送**  - 最新のハードウェア機能で最適化されたコーディング処理
-  **インテリジェントペアリング**  - ペアリングデバイスの対応する構成ファイルをインテリジェントに管理する

###🖥§仮想モニター統合（Win10 22H2および新しいシステムが必要）
- 動的仮想ディスプレイの作成と破壊
- カスタム解像度とリフレッシュレートのサポート
- マルチディスプレイ構成管理
- 再起動せずにリアルタイムの構成変更


##推奨ムーンライトクライアント

最高のストリーミングエクスペリエンス（セットプロパティをアクティブ化）に次の最適化されたムーンライトクライアントを使用することをお勧めします。

###🖥🖥§Windows（x86_64、arm64）、macos、linuxクライアント
[！[ムーンライト-pc]（https://img.shields.io/badge/moonlight-pc-red?style=for-the-badge&logo=windows）]（https://github.com/qiin2333/moonlight-qt）

### androidクライアント
[。
[！[CrownバージョンMoonlight-android]（https://img.shields.io/badge/crown version-moonlight-android-blue？style = for-the-badge＆logo = android）]（https://github.com/wacrown/moon-android））

###📱iOSクライアント
[！[Real Brick Edition Moonlight-Ios]（https://img.shields.io/badge/real Brick Edition-moonlight--- Ios-lightgrey？style = for-the-badge＆Logo = apple）]


###
[Awesome-Sunshine]（https://github.com/lizardbyte/awesome-sunshine）

##システム要件


> [！警告]
>これらのテーブルは継続的に更新されています。この情報のみに基づいてハードウェアを購入しないでください。


<表>
<キャプションID = "Minimut_Requirements">最小構成要件</caption>
<tr>
<th>コンポーネント</th>
<th>要件</th>
</tr>
<tr>
<td rowspan = "3"> gpu </td>
<td> amd：vce 1.0以降、<a href = "https://github.com/obsproject/obs-amd-encoder/wiki/hardware-support"> obs-amdハードウェアサポート</a> </td>
</tr>
<tr>
<td> intel：vaapi互換性、<a href = "https://www.intel.com/content/www/us/en/developer/articles/technical/linuxmedia-vaapi.html"> vaapi Hardware support </a> </td>
</tr>
<tr>
<td> nvidia：nvenc-supportedグラフィックスカード、<a href = "https://developer.nvidia.com/video-encode-and-decode-gpu-support-new"> nvencサポートmatrix </a> </td>を参照してください
</tr>
<tr>
<td rowspan = "2"> cpu </td>
<td> amd：ryzen 3以降</td>
</tr>
<tr>
<td> intel：core i3以降</td>
</tr>
<tr>
<td> ram </td>
<td> 4GB以上</td>
</tr>
<tr>
<td rowspan = "5">オペレーティングシステム</td>
<TD> Windows：10 22H2+（Windows Serverは仮想ゲームパッドをサポートしていません）</td>
</tr>
<tr>
<td> macos：12+</td>
</tr>
<tr>
<td> linux/debian：12+（bookworm）</td>
</tr>
<tr>
<td> linux/fedora：39+</td>
</tr>
<tr>
<td> linux/ubuntu：22.04+（ジャミー）</td>
</tr>
<tr>
<td rowspan = "2"> network </td>
<TD>ホスト：5GHz、802.11ac </td>
</tr>
<tr>
<TD>クライアント：5GHz、802.11ac </td>
</tr>
</table>

<表>
<キャプションID = "4K_SUGGESTIONS"> 4K推奨設定</caption>
<tr>
<th>コンポーネント</th>
<th>要件</th>
</tr>
<tr>
<td rowspan = "3"> gpu </td>
<TD> AMD：ビデオコーディングエンジン3.1以降</td>
</tr>
<tr>
<td> intel：HDグラフィック510以降</td>
</tr>
<tr>
<td> nvidia：geforce gtx 1080以下のマルチエンコーダー</td>
</tr>
<tr>
<td rowspan = "2"> cpu </td>
<td> amd：ryzen 5以降</td>
</tr>
<tr>
<td> intel：core i5以降</td>
</tr>
<tr>
<td rowspan = "2"> network </td>
<TD>ホスト：CAT5Eイーサネットまたはそれ以上</td>
</tr>
<tr>
<TD>クライアント：CAT5Eイーサネットまたはそれ以上</td>
</tr>
</table>

＃＃ テクニカルサポート

問題に遭遇したときのソリューションパス：
1。view[user document]（https://docs.qq.com/aio/dsgdqc3htbfjjsfdo?p=ytpmj5jnndb5hekjjhhqlsb）[lizardbyte document]（https://docs.lizardbyte.dev/project/projects/sunshine/-sunshine/）
2。関連情報を見つけるために、設定で詳細なログレベルを開きます
3。[QQ通信グループに参加してヘルプを得る]（https://qm.qq.com/cgi-bin/qm/qr?k=5qnkzsaliriau4fvumftzh_6hg7fuuld&jump_from=webapi）
4。[2文字を使用してください！]（https://uuyc.163.com/）

**問題のフィードバックタグ：**
-  `HDR -Support` -HDR関連の問題
-  `Virtual -display` -Virtual Displayの問題
-  `config -help` -Configuration関連の問題

##コミュニティに参加します

ディスカッションに参加し、コードに貢献することを皆さんを歓迎します！
[！[QQ Groupに参加]（https://pub.idqqimg.com/wpa/images/group.png '' Join qqグループ '）]（https://qm.qq.com/cgi-bin/qm/qr?k=wc2psz3q6hk6j8u_dg9s7522gptitk0m&jump_fr om = webapi＆authkey = zvdlfrs83s/0xg3hmbkmeaqi7xohxam3sxzif/u9jw7qo/d8xd0npytvbc2los+z）

##スターの歴史

[！[スターヒストリーチャート]（https://api.star-history.com/svg?repos=qiin2333/sunshine-foundation&type=date）]

---

**サンシャインベースエディション - ゲームストリーミングを簡単にする**