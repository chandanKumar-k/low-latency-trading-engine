import tkinter as tk
import customtkinter as ctk
import subprocess
import threading

ctk.set_appearance_mode("Dark")
ctk.set_default_color_theme("blue")

class TradingDashboard(ctk.CTk):
    def __init__(self):
        super().__init__()

        self.title("PESU High-Frequency Order Placement Terminal v4.0")
        self.geometry("950x600")

        # Start the continuous background C++ core process pipeline
        self.cpp_process = subprocess.Popen(
            ["./engine_api"], 
            stdin=subprocess.PIPE, 
            stdout=subprocess.PIPE, 
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1
        )

        # --- LEFT PANEL: CONTROLS ---
        self.control_frame = ctk.CTkFrame(self, width=260, corner_radius=10)
        self.control_frame.pack(side="left", fill="y", padx=15, pady=15)

        self.title_label = ctk.CTkLabel(self.control_frame, text="ORDER CONFIGURATION", font=ctk.CTkFont(size=14, weight="bold"))
        self.title_label.pack(padx=10, pady=15)

        self.ticker_input = ctk.CTkEntry(self.control_frame, placeholder_text="Ticker Symbol (e.g. INFY)")
        self.ticker_input.pack(fill="x", padx=15, pady=10)

        self.type_menu = ctk.CTkOptionMenu(self.control_frame, values=["BUY", "SELL"])
        self.type_menu.pack(fill="x", padx=15, pady=10)

        self.price_input = ctk.CTkEntry(self.control_frame, placeholder_text="Limit Target Price (Rs.)")
        self.price_input.pack(fill="x", padx=15, pady=10)

        self.qty_input = ctk.CTkEntry(self.control_frame, placeholder_text="Share Trading Volume")
        self.qty_input.pack(fill="x", padx=15, pady=10)

        self.submit_btn = ctk.CTkButton(self.control_frame, text="Inject into C++ Core", command=self.send_order_to_cpp)
        self.submit_btn.pack(fill="x", padx=15, pady=15)

        # --- RIGHT PANEL: LIVE DISPLAY PANELS ---
        self.right_frame = ctk.CTkFrame(self)
        self.right_frame.pack(side="right", fill="both", expand=True, padx=15, pady=15)

        # Split top area to visually separate BUY and SELL book backlogs
        self.book_display_frame = ctk.CTkFrame(self.right_frame, height=250)
        self.book_display_frame.pack(fill="both", expand=True, padx=10, pady=5)

        self.buy_book_box = ctk.CTkTextbox(self.book_display_frame, width=300, font=ctk.CTkFont(family="Courier", size=11))
        self.buy_book_box.pack(side="left", fill="both", expand=True, padx=5, pady=5)
        self.buy_book_box.insert("end", "=== ACTIVE BUY QUEUE (HIGHEST FIRST) ===\n(Empty)\n")

        self.sell_book_box = ctk.CTkTextbox(self.book_display_frame, width=300, font=ctk.CTkFont(family="Courier", size=11))
        self.sell_book_box.pack(side="right", fill="both", expand=True, padx=5, pady=5)
        self.sell_book_box.insert("end", "=== ACTIVE SELL QUEUE (LOWEST FIRST) ===\n(Empty)\n")

        # Bottom logging terminal for trade execution alerts
        self.console_output = ctk.CTkTextbox(self.right_frame, height=200, font=ctk.CTkFont(family="Courier", size=12))
        self.console_output.pack(fill="x", padx=10, pady=10)
        self.console_output.insert("end", "[SYSTEM NOTICE]: Continuous stream bridge bound to stateful C++ core.\n\n")

        # Launch listener thread loop to handle real-time background outputs from C++
        threading.Thread(target=self.listen_to_cpp, daemon=True).start()

    def send_order_to_cpp(self):
        ticker = self.ticker_input.get().upper()
        order_type = self.type_menu.get()
        price = self.price_input.get()
        qty = self.qty_input.get()

        if not ticker or not price or not qty:
            return

        # Push order details directly down the data input pipe straight into the living C++ process memory loop
        payload = f"ADD {ticker} {order_type} {price} {qty}\n"
        self.cpp_process.stdin.write(payload)
        self.cpp_process.stdin.flush()

    def request_book_update(self):
        # 1. Force clear the visual UI display panels right away
        self.buy_book_box.delete("1.0", "end")
        self.buy_book_box.insert("end", "=== ACTIVE BUY QUEUE (HIGHEST FIRST) ===\n")
        
        self.sell_book_box.delete("1.0", "end")
        self.sell_book_box.insert("end", "=== ACTIVE SELL QUEUE (LOWEST FIRST) ===\n")
        
        # 2. Ping C++ core to fetch fresh records
        try:
            self.cpp_process.stdin.write("VIEW\n")
            self.cpp_process.stdin.flush()
        except Exception as e:
            self.console_output.insert("end", f"[PIPE ERROR]: Failed to signal backend core: {str(e)}\n")

    def listen_to_cpp(self):
        in_book_mode = False
        buy_lines = []
        sell_lines = []

        while True:
            line = self.cpp_process.stdout.readline().strip()
            if not line:
                break

            if "MATCH_SUCCESS" in line:
                _, b_id, s_id, stock, prc, count = line.split("|")
                self.console_output.insert("end", f"⚡ MATCH EXECUTED: {count} shares of {stock} transacted at Rs. {prc} ({b_id} <-> {s_id})\n")
                self.request_book_update() # Instantly refresh books upon completion events
            
            elif "NO_MATCH" in line:
                self.console_output.insert("end", f"[SYSTEM]: Order placed safely in active storage memory logs.\n")
                self.request_book_update()

            elif line == "BOOK_START":
                in_book_mode = True
                buy_lines = ["=== ACTIVE BUY QUEUE (HIGHEST FIRST) ===\n"]
                sell_lines = ["=== ACTIVE SELL QUEUE (LOWEST FIRST) ===\n"]
            
            elif line == "BOOK_END":
                in_book_mode = False
                # Safely update display panes inside standard UI layout threads
                self.buy_book_box.delete("1.0", "end")
                self.buy_book_box.insert("end", "".join(buy_lines) if len(buy_lines) > 1 else "=== ACTIVE BUY QUEUE (HIGHEST FIRST) ===\n(Empty)\n")
                
                self.sell_book_box.delete("1.0", "end")
                self.sell_book_box.insert("end", "".join(sell_lines) if len(sell_lines) > 1 else "=== ACTIVE SELL QUEUE (LOWEST FIRST) ===\n(Empty)\n")

            elif in_book_mode:
                parts = line.split("|")
                formatted_line = f"ID: {parts[1]} | {parts[2]} | Price: Rs. {parts[3]} | Qty: {parts[4]}\n"
                if parts[0] == "BUY":
                    buy_lines.append(formatted_line)
                else:
                    sell_lines.append(formatted_line)

if __name__ == "__main__":
    app = TradingDashboard()
    app.mainloop()
