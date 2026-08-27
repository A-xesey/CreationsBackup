**CreationsBackup** is the mod that adds backup possibility in editors and adventure editor.

Backups are saving in `AppData\Roaming\Spore\Backups` path by their type (Adventures, Creatures...), creation name and numeration
**Be careful!** If creation hasn't the name, the mod will create a folder with creation ID and send backups here

Creating a `AppData\Roaming\Spore\Preferences\BackupSettings.prop` file that needs for backup control:
`BackupByStepADV` and `BackupByStepEDT` determines how many steps to take before making a backup
`BackupEnable` is the mod's switcher
`EditorAllowXMLFileDrop` in-game property using for importing in the editor valid creation formats

**Also mod has:**
* The `backupcreations` cheat to set the BackupSettings.prop values from console
* Import in the game the creation formats (.crt, .vcl, .ufo, etc.) without name + .bem and .adv formats
