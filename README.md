# 🏦 Enterprise Banking System v6.0

[![C++17](https://img.shields.io/badge/C%2B%2B-17%2B-blue.svg)](https://isocpp.org/) 
[![GitHub license](https://img.shields.io/github/license/yourusername/yourrepo)](LICENSE)
[![Issues](https://img.shields.io/github/issues/yourusername/yourrepo.svg)](https://github.com/yourusername/yourrepo/issues)
[![Contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg)](CONTRIBUTING.md)

A production-ready C++ banking system simulating real-world operations with professional reliability and precision.

---

## 💻 Quick Deployment

<details>
<summary>🚀 How to Build & Run</summary>

**Linux/macOS**
g++ -std=c++17 -pthread -o banking_system enterprise-banking-v6-production-fixes.cpp

**Windows (MSVC)**
cl /std:c++17 /EHsc enterprise-banking-v6-production-fixes.cpp /Fe:banking_system.exe

**Run:**
./banking_system

On first run, the following folder structure is auto-created:
data/
logs/
backups/
config/
</details>

---

## ⚙️ Core Features

- Account Management
- Transaction Processing (Deposit, Withdrawal, Transfer, EMI)
- PIN-authentication & Secure Password Hashing (Salted, Configurable)
- Transaction History (full audit trail with UTC timestamps)
- Atomic file operations and auto-backup
- ACID-compliant transaction rollback
- INI-based configuration management
- Thread-safe professional logging (multi-level audit)
- Supports GCC 8+/Clang 7+/MSVC 2019+, C++17+

---

## ✨ Production Fixes

<details>
<summary>Click for Details on Fixes</summary>

### Critical Fix #1 – Monetary Precision
- Integer-based paise arithmetic (prevents floating-point errors)

### Critical Fix #2 – UTC Timestamps
- Consistent, UTC-labeled transaction history

### Critical Fix #3 – Enhanced Atomic Operations
- Automatic backup and rollback
- Integrity protection & file system checks

### Critical Fix #4 – Transaction Rollback
- ACID-compliant, restores account state on failure

</details>

---

## 🛡️ Security & Reliability

- Password hashing with salts
- PIN validation logic
- Transaction rollback for full ACID compliance
- Multi-level, timestamped logs for audit
- Atomic file operations for persistent safety

---

## 📦 Configuration Example

Edit `config/banking.ini` for customized behavior:
[account]
min_account_number=100001
max_account_number=999999
starting_account_number=100000

[security]
max_pin_attempts=3
pin_length=4
min_password_length=6

[transaction]
max_history_per_account=500
large_transaction_threshold=50000

---

## 📂 Directory Structure
<pre>
/
├── enterprise-banking-v6-production-fixes.cpp
├── data/
│ ├── accounts.txt
│ └── transactions_[account_number].txt
├── logs/
│ └── banking_[timestamp]_UTC.log
├── backups/
├── config/
│ └── banking.ini
└── README.md
</pre>

---

## 🌗 Theme Compatibility

This README is styled for visibility in both light and dark modes on GitHub and GitHub Pages.  
For a custom Pages site, choose a supported [Jekyll theme](https://docs.github.com/en/pages/setting-up-a-github-pages-site-with-jekyll/adding-a-theme-to-your-github-pages-site-using-jekyll) and adapt `_config.yml` as needed.

---

## 📘 License

This project can be used for educational, research, and foundational implementation purposes in financial software systems.

© 2025 Enterprise Banking System v6.0 | Precision. Reliability. Security.
