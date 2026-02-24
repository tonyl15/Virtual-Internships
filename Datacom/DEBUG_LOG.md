# DEBUG_LOG.md - AI Collaboration Example

## Task 1: AI-Powered Debugging and Refactoring

**Date**: January 31, 2026  
**Student**: me!
**Task**: Fix legacy data processing script and validate improvements

---

## Step 1: Understanding the Codebase

### Prompt to AI:

```
This is a legacy Python script. Please provide a high-level summary of what it's supposed to do. Then, break down your explanation function by function, detailing the purpose of each, its expected inputs and outputs, and any side effects. Finally, identify any potential areas of concern or parts of the code that seem overly complex or inefficient.
```

### AI Response:

```
High-level Summary:
The script loads customer records and transactions from CSV, updates per-customer totals, calculates aggregate metrics, generates JSON reports, and exports customer data.

Function Breakdown:
1. DataProcessor.__init__(): Sets input file and initializes in-memory structures
2. load_data(): Reads customers CSV into self.customers
3. process_transactions(): Reads transactions CSV and updates customer totals
4. calculate_customer_metrics(): Calculates revenue, averages, top customers, category breakdown
5. find_matches(): Searches customers by a specified field
6. generate_report(): Writes JSON reports for summary, metrics, or transactions
7. export_customer_data(): Writes customer export in CSV/JSON

Areas of Concern:
- Hard-coded filenames in main()
- Limited input validation
- Potential data-shape assumptions in export logic
```

### My Notes:

The summary clarified the flow and highlighted the export area as a likely risk, which helped guide debugging.

---

## Step 2: Diagnosing the Specific Bug

### Prompt to AI:

```
from the script and the associated error log, what is the most likely root cause of the failure? Please explain your reasoning step-by-step, referencing specific lines of code and the error message.
```

### AI Response:

```
The error points to export_customer_data() where fieldnames are derived using .keys() on the first customer record. If that record isn't a dict, the call fails. The error log matches this path, making export_customer_data() the most likely source of the failure.
```

### My Notes:

Mapping the error to the only .keys() usage gave a clear target for a fix and a repro test.

---

## Step 3: Writing a Test to Replicate the Bug

### Prompt to AI:

```
Based on your analysis of the bug, write a Python unit test using the 'unittest' library that is specifically designed to fail in the same way the error log shows. This test should call the problematic function with data that triggers the bug. Save the generated code into a new file named TEST_CASES.py.
```

### AI Response:

```
Created a unittest that constructs a DataProcessor instance and injects a customer record that triggers the export failure, then calls export_customer_data() to reproduce the error path.
```

### My Notes:

The test isolates export_customer_data() and forces the invalid record shape to reproduce the failure.

---

## Step 4: Refactoring and Fixing the Code

### Prompt to AI:

```
Refactor this function to fix the bug we identified. While doing so, also improve its performance. The current implementation uses inefficient nested for-loops; please replace this logic with a more performant method, such as using a dictionary lookup.
```

### AI Response:

```
Added a type check for the first customer record before calling .keys().
Simplified CSV export by using writerows with a generator to reduce per-row overhead.
```

### My Notes:

The guard prevents the AttributeError, and writerows improves throughput by avoiding repeated writer calls.

---

## Step 5: Test Updated to Match New Behavior

### Prompt to AI:

```
when i run the test case i get ERROR:process_data:Customer records must be dicts to export to CSV
```

### AI Response:

```
Updated the unit test to assert that the function returns False and logs the expected error message instead of raising an exception.
```

### My Notes:

The test now reflects the fixed behavior and validates both the return value and the log output.
