### Notes : 

* to hide the status bar in ios, add this into the build's `<AppName>-Info.plist` file :
```
<key>UIStatusBarHidden</key>
<true/>
<key>UIViewControllerBasedStatusBarAppearance</key>
<false/>
```
* to solve the xcassets error when building the app for ios, see : https://stackoverflow.com/questions/36095819/cordova-ios-error-building-images-xcassets
