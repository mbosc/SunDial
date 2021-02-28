module.exports = [
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Debug"
      },
      {
        "type": "text",
        "defaultValue": "Toggle the switch below to set SunDial time to a custom value (this disables automatic time updates)"
      },
      {
        "type": "toggle",
        "messageKey": "SHOW_MODE",
        "defaultValue": false,
        "label": "Demo Mode"
      },
      {
        "type": "slider",
        "messageKey": "CUSTOMTIME",
        "defaultValue": 15,
        "label": "Custom Time",
        "description": "Set a custom time (in hours)",
        "min": 0,
        "max": 23.75,
        "step": 0.25
      },

    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];