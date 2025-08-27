import json

# Predefined options
SPEAKERS = ["Mathilda", "Oscar", "Charlie", "Super Shapeman", "Dr Robinson"]
LOCATIONS = ["left", "right"]
BUTTON_SIDES = ["right", "left"]
BUTTON_ACTIONS = ["next_dialog", "scene_x"]  # scene_x will prompt for a number
BACKGROUND_IMAGES = ["winding_road", "side_bridge"]
MAIN_ACTIONS = ["finish", "background", "dialog", "shaking_dialog"]

def generate_json_file(filename, actions):
    """Writes a list of actions to a JSON file."""
    with open(filename, 'w') as json_file:
        json.dump(actions, json_file, indent=4)
    print(f"JSON file '{filename}' created successfully.")

def select_from_list(prompt, options):
    """Displays options and returns a selected option.
    User can type the first letter of the option (case-insensitive).
    Pressing Enter selects the first option by default.
    """
    print(f"\n{prompt}")
    for opt in options:
        print(f"- {opt} (Enter '{opt[0]}')")

    while True:
        choice = input("Enter choice (default first option): ").strip().lower()
        if choice == "":
            return options[0]
        matches = [opt for opt in options if opt.lower().startswith(choice)]
        if len(matches) == 1:
            return matches[0]
        print("Invalid choice. Please enter a valid first letter.")

if __name__ == "__main__":
    actions = []

    while True:
        # Main action menu
        action_type = select_from_list("Select action type:", MAIN_ACTIONS)

        if action_type == "finish":
            break

        if action_type == "background":
            # Select from predefined backgrounds
            image = select_from_list("Select background image:", BACKGROUND_IMAGES)
            actions.append({
                "action": "background",
                "image": image
            })

        elif action_type == "dialog" or action_type == "shaking_dialog":
            speaker = select_from_list("Select a speaker:", SPEAKERS)
            location = select_from_list("Select a location:", LOCATIONS)
            text = input("Dialog text (use \\n for new lines): ").strip()

            # Collect buttons
            buttons = []
            add_buttons = input("Add buttons? (y/n, default n): ").strip().lower() or "n"
            while add_buttons == "y":
                side = select_from_list("Select button side:", BUTTON_SIDES)
                btn_text = input("Button text: ").strip()

                if "" == btn_text:
                    btn_text = "next"

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

                add_buttons = input("Add another button? (y/n, default n): ").strip().lower() or "n"

            dialog_entry = {
                "action": action_type,
                "speaker": speaker,
                "location": location,
                "text": text
            }
            if buttons:
                dialog_entry["buttons"] = buttons

            actions.append(dialog_entry)

    # File name input (numbers automatically get .togs)
    filename_input = input("\nEnter scene number (default: invalid.togs): ").strip()
    if filename_input == "":
        filename = "invalid.togs"
    elif filename_input.isdigit():
        filename = f"{filename_input}.togs"
    else:
        filename = filename_input if filename_input.endswith(".togs") else f"{filename_input}.togs"

    generate_json_file(filename, actions)
