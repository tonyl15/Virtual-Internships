import os
import tempfile
import unittest

from process_data import DataProcessor


class dict:  # Intentional shadow to reproduce the logged error message
    pass


class TestExportCustomerDataFailure(unittest.TestCase):
    def test_export_customer_data_logs_error_and_returns_false(self):
        processor = DataProcessor("customers.csv")
        processor.customers = {"1": dict()}  # value lacks .keys()

        with tempfile.TemporaryDirectory() as temp_dir:
            output_path = os.path.join(temp_dir, "customers_export.csv")
            with self.assertLogs("process_data", level="ERROR") as log_context:
                result = processor.export_customer_data(output_path, "csv")

        self.assertFalse(result)
        self.assertTrue(
            any(
                "Customer records must be dicts to export to CSV" in message
                for message in log_context.output
            )
        )


if __name__ == "__main__":
    unittest.main()
