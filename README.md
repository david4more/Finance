# CoinWarden – Finance Manager

A Qt/C++ Finance tracking app built with MVC architecture, it provides dashboards, 
transactions filtering, multi-currency support with automatic conversion and more.

## Features

- Visual analysis of your recent activity
- Finances tracking with customizable filters
- Automatic currency conversion
- Monitoring expense limits per category
- Fetching latest exchange rates from the web

## Screenshots
### Home page
<img width="1282" height="832" alt="Image" src="https://github.com/user-attachments/assets/5170538e-720d-4f4c-bdb8-0be1cf202b07" />

### Configuring custom filters with a window-modal dialog
<img width="1281" height="832" alt="Image" src="https://github.com/user-attachments/assets/7e72e537-9fe1-4131-9c64-92a4662c1d91" />

### Settings page
<img width="1282" height="832" alt="Image" src="https://github.com/user-attachments/assets/acde4986-9ac4-4954-aafb-53f04ca54b51" />

More screenshots: [Issues → Screenshots](https://github.com/david4more/CoinWarden/issues/1)


## Roadmap

- Complete home page data displaying in base currency
- Finish currencies request implementation
- Editing support for categories/accounts/transactions
- Display categories' colors and accounts' icons
- Rewrite business logic with STL

## Long-term vision
- Remote database integration, accounts management
- Mobile version

## Build
To run the project, you need a [currencyapi](https://app.currencyapi.com) key. Environment variables:

`CURRENCY_API_KEY=your_key`

To launch the project outside Qt Creator, add the following to your configuration's 
### Environment variables:
```
PATH=(path to Qt)\ Qt \ 6.10.1 \ *Your CPP compiler* \ _64 \ bin;
(path to the repo)\ Desktop \ QCustomPlot;
(path to the repo)\ *Your build directory* \ Backend
```
### CMake options:
`-D CMAKE_PREFIX_PATH="(path to Qt)/Qt/6.10.1/Your CPP compiler/lib/cmake`

## Other
This project uses [QCustomPlot](https://www.qcustomplot.com/) for charts.

Icons, used in the app, are from: www.flaticon.com/authors/pixel-perfect
