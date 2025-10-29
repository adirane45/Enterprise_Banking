<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Enterprise Banking System v6.0 - Production Ready</title>
<style>
  body {
    font-family: "Segoe UI", Arial, sans-serif;
    margin: 0;
    background: #f6f8fb;
    color: #222;
    line-height: 1.6;
  }
  header {
    background: linear-gradient(135deg, #004c97, #0073e6);
    color: #fff;
    padding: 40px 20px;
    text-align: center;
  }
  header h1 {
    font-size: 2rem;
    margin-bottom: 10px;
  }
  header p {
    font-size: 1.1rem;
    opacity: 0.9;
  }
  section {
    background: #fff;
    margin: 20px auto;
    max-width: 900px;
    padding: 25px 30px;
    border-radius: 10px;
    box-shadow: 0 4px 12px rgba(0,0,0,0.08);
  }
  h2 {
    color: #004c97;
    border-left: 4px solid #0073e6;
    padding-left: 10px;
    font-size: 1.4rem;
  }
  ul {
    list-style: none;
    padding-left: 0;
  }
  ul li {
    padding: 6px 0;
  }
  code {
    background: #eef3f7;
    padding: 2px 6px;
    border-radius: 4px;
    font-size: 0.95em;
  }
  .config-box {
    background: #f5faff;
    border-left: 4px solid #0073e6;
    padding: 10px 15px;
    font-family: Consolas, monospace;
    margin: 10px 0;
    border-radius: 6px;
  }
  footer {
    text-align: center;
    font-size: 0.9rem;
    color: #777;
    margin: 40px 0;
  }
</style>
</head>
<body>

<header>
  <h1>🏦 Enterprise Banking System v6.0</h1>
  <p>Production-Ready C++ Banking Simulation with Real-World Reliability</p>
</header>

<section>
  <h2>💡 Overview</h2>
  <p>This system simulates real-world banking operations with professional-grade precision and reliability. It includes critical production fixes ensuring financial integrity, security, and performance.</p>
</section>

<section>
  <h2>⚙️ Core Functionality</h2>
  <ul>
    <li><b>Account Management:</b> Create and manage Savings, Current, and Loan accounts.</li>
    <li><b>Transaction Processing:</b> Deposits, withdrawals, transfers, and EMI payments.</li>
    <li><b>User Authentication:</b> PIN and password-based access with secure hashing.</li>
    <li><b>Transaction History:</b> Complete audit trail with UTC timestamps.</li>
    <li><b>File Operations:</b> Persistent data storage using atomic operations.</li>
    <li><b>Configuration Management:</b> Flexible runtime settings via INI files.</li>
    <li><b>Professional Logging:</b> Thread-safe, multi-level logs with timestamps.</li>
  </ul>
</section>

<section>
  <h2>✨ Production Fixes</h2>
  <ul>
    <li><b>Fix #1 – Monetary Precision:</b> Integer-based arithmetic (paise) eliminates rounding errors and ensures overflow protection.</li>
    <li><b>Fix #2 – UTC Timestamps:</b> Consistent time synchronization with UTC-based labeling.</li>
    <li><b>Fix #3 – Enhanced Atomic Operations:</b> Automatic rollback, integrity validation, and file safety.</li>
    <li><b>Fix #4 – Transaction Rollback:</b> ACID-compliant recovery ensuring error-free transaction management.</li>
  </ul>
</section>

<section>
  <h2>💻 System Requirements</h2>
  <ul>
    <li><b>C++ Standard:</b> C++17 or higher (requires <code>std::filesystem</code>)</li>
    <li><b>Supported Compilers:</b> GCC 8.0+, Clang 7.0+, MSVC 2019+</li>
  </ul>
</section>

<section>
  <h2>🚀 Deployment Guide</h2>
  <h3>Step 1: Compilation</h3>
  <p><b>Linux/macOS:</b></p>
  <code>g++ -std=c++17 -pthread -o banking_system enterprise-banking-v6-production-fixes.cpp</code><br><br>
  <p><b>Windows (MSVC):</b></p>
  <code>cl /std:c++17 /EHsc enterprise-banking-v6-production-fixes.cpp /Fe:banking_system.exe</code>

  <h3>Step 2: Initialization</h3>
  <p>The program automatically creates directory structures for <code>data/</code>, <code>logs/</code>, <code>backups/</code>, and <code>config/</code>, initializes logging, and generates default configurations.</p>

  <h3>Step 3: Execution</h3>
  <code>./banking_system</code>
  <p>Use the guided interface for account creation, secure login, and transaction operations.</p>

  <h3>Step 4: Configuration</h3>
  <div class="config-box">
    [account]<br>
    min_account_number=100001<br>
    max_account_number=999999<br>
    starting_account_number=100000<br><br>
    [security]<br>
    max_pin_attempts=3<br>
    pin_length=4<br>
    min_password_length=6<br><br>
    [transaction]<br>
    max_history_per_account=500<br>
    large_transaction_threshold=50000
  </div>

  <h3>Step 5: Data Management</h3>
  <ul>
    <li>Account Data: <code>data/accounts.txt</code></li>
    <li>Transaction Logs: <code>data/transactions_[account_number].txt</code></li>
    <li>System Logs: <code>logs/banking_[timestamp]_UTC.log</code></li>
    <li>Backups: <code>backups/</code></li>
  </ul>
</section>

<section>
  <h2>🛡️ Production Features</h2>
  <ul>
    <li><b>Security:</b> Password hashing with salts and protected PIN validation.</li>
    <li><b>Integrity:</b> Full ACID transaction compliance with rollback.</li>
    <li><b>Monitoring:</b> Thread-safe professional logging for audits.</li>
    <li><b>Scalability:</b> Concurrent access handling with thread safety.</li>
    <li><b>Reliability:</b> Atomic file writes for persistent and safe operations.</li>
  </ul>
</section>

<footer>
  © 2025 Enterprise Banking System v6.0 | Built for Real-World Financial Applications
</footer>

</body>
</html>
