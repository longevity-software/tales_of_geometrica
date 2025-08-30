import json
import sys
import os

# -------- Platform-specific single-key input --------
if os.name == "nt":  # Windows
    import msvcrt
    def getch():
        return msvcrt.getch().decode("utf-8", errors="ignore")
else:  # Unix (Linux / macOS)
    import tty
    import termios
    def getch():
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(fd)
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch

# -------- Options --------
SPEAKERS = ["Mathilda", "Oscar", "Charlie", "Super Shapeman", "Dr Robinson"]
LOCATIONS = ["left", "right"]
BUTTON_SIDES = ["right", "left"]
BUTTON_ACTIONS = ["next_dialog", "scene_x"]
BACKGROUND_IMAGES = ["winding_road", "side_bridge"]
MAIN_ACTIONS = ["finish", "background", "dialog", "shaking_dialog", "menu", "remove_last"]

# -------- Helpers --------
def generate_json_file(filename, actions):
    with open(filename, 'w') as json_file:
        json.dump(actions, json_file, indent=4)
    print(f"✅ JSON file '{filename}' created successfully.")

def select_from_list(prompt, options):
    print(f"\n{prompt}")
    for opt in options:
        print(f"- {opt} (Press '{opt[0]}')")

    while True:
        choice = getch().lower()
        if choice in ("\r", "\n"):  # Enter → default
            print(f" -> {options[0]}")
            return options[0]
        matches = [opt for opt in options if opt.lower().startswith(choice)]
        if len(matches) == 1:
            print(f" -> {matches[0]}")
            return matches[0]
        else:
            print("Invalid choice, try again.")

def yes_no_prompt(prompt, default="n"):
    """Single-key y/n prompt without pressing Enter."""
    print(f"{prompt} (y/n, default {default})")
    while True:
        ch = getch().lower()
        if ch in ("y", "n"):
            print(f" -> {ch}")
            return ch
        elif ch in ("\r", "\n"):
            print(f" -> {default}")
            return default

def load_existing_scene():
    fname = input("\nEnter existing .togs filename: ").strip()
    if not fname.endswith(".togs"):
        fname += ".togs"
    if not os.path.exists(fname):
        print("⚠️ File not found, starting fresh.")
        return []
    with open(fname, "r") as f:
        try:
            return json.load(f)
        except json.JSONDecodeError:
            print("⚠️ Invalid JSON in file, starting fresh.")
            return []

# -------- Main Loop --------
if __name__ == "__main__":
    while True:  # Outer loop for "new scene after save"
        print("\n--- Scene Builder ---")
        load_choice = yes_no_prompt("Load existing scene?", default="n")
        if load_choice == "y":
            actions = load_existing_scene()
        else:
            actions = []

        while True:  # Inner loop for building one scene
            action_type = select_from_list("Select action type:", MAIN_ACTIONS)

            if action_type == "finish":
                break

            if action_type == "remove_last":
                if actions:
                    removed = actions.pop()
                    print(f"🗑️ Removed last action: {removed}")
                else:
                    print("⚠️ No actions to remove.")
                continue

            if action_type == "background":
                image = select_from_list("Select background image:", BACKGROUND_IMAGES)
                actions.append({
                    "action": "background",
                    "image": image
                })

            elif action_type in ("dialog", "shaking_dialog"):
                speaker = select_from_list("Select a speaker:", SPEAKERS)
                location = select_from_list("Select a location:", LOCATIONS)
                text = input("Dialog text (use \\n for new lines): ").strip()

                # Collect buttons
                buttons = []
                add_buttons = yes_no_prompt("Add buttons?", default="n")
                while add_buttons == "y":
                    side = select_from_list("Select button side:", BUTTON_SIDES)
                    btn_text = input("Button text (default 'next'): ").strip() or "next"

                    # Choose action
                    action_choice = select_from_list("Select button action:", BUTTON_ACTIONS)
                    if action_choice == "scene_x":
                        num = input("Enter scene number (integer): ").strip()
                        while not num.isdigit():
                            num = input("Please enter a valid integer: ").strip()
                        btn_action = f"scene_{num}"
                    else:
                        btn_action = action_choice

                    buttons.append({
                        "side": side,
                        "text": btn_text,
                        "action": btn_action
                    })

                    add_buttons = yes_no_prompt("Add another button?", default="n")

                dialog_entry = {
                    "action": action_type,
                    "speaker": speaker,
                    "location": location,
                    "text": text
                }
                if buttons:
                    dialog_entry["buttons"] = buttons
                actions.append(dialog_entry)

            elif action_type == "menu":
                # Standalone menu with buttons only
                buttons = []
                print("\n📋 Creating a menu...")
                add_buttons = "y"
                while add_buttons == "y":
                    btn_text = input("Button text (default 'next'): ").strip() or "next"

                    # Choose action
                    action_choice = select_from_list("Select button action:", BUTTON_ACTIONS)
                    if action_choice == "scene_x":
                        num = input("Enter scene number (integer): ").strip()
                        while not num.isdigit():
                            num = input("Please enter a valid integer: ").strip()
                        btn_action = f"scene_{num}"
                    else:
                        btn_action = action_choice

                    buttons.append({
                        "text": btn_text,
                        "action": btn_action
                    })

                    add_buttons = yes_no_prompt("Add another menu button?", default="n")

                if buttons:
                    actions.append({
                        "action": "menu",
                        "buttons": buttons
                    })

        # Save scene
        filename_input = input("\nEnter scene number (default: invalid.togs): ").strip()
        if filename_input == "":
            filename = "invalid.togs"
        elif filename_input.isdigit():
            filename = f"{filename_input}.togs"
        else:
            filename = filename_input if filename_input.endswith(".togs") else f"{filename_input}.togs"

        generate_json_file(filename, actions)

        # Ask to build another
        again = yes_no_prompt("\nStart a new scene?", default="n")
        if again != "y":
            print("👋 Done.")
            break
