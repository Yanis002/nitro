import argparse
from pathlib import Path
import json

parser = argparse.ArgumentParser(description="Merges many build/<game>/objdiff.json files into one")
parser.add_argument("games", nargs="+")
args = parser.parse_args()

games: list[str] = args.games

current_path = Path(__name__)
root_path = current_path.parent.resolve()
output_path = root_path / "objdiff.json"


class Project:
    def __init__(self, *, game: str, objdiff: dict, path: Path):
        self.game = game
        self.objdiff = objdiff
        self.path = path

    def rebase_relative_path(self, old_path: str):
        return (self.path / old_path).resolve().relative_to(root_path)


projects = []
for game in games:
    path = root_path / "build" / game
    objdiff_json = path / "objdiff.json"
    with objdiff_json.open("r") as f:
        objdiff: dict = json.loads(f.read())
    projects.append(Project(game=game, objdiff=objdiff, path=path))

result = {}
result["min_version"] = projects[0].objdiff["min_version"]
result["custom_make"] = projects[0].objdiff["custom_make"]
result["build_base"] = projects[0].objdiff["build_base"]
result["build_target"] = projects[0].objdiff["build_target"]
result["watch_patterns"] = projects[0].objdiff["watch_patterns"]


units = []

for project in projects:
    for old_unit in project.objdiff["units"]:
        unit = {}
        unit["name"] = f"{project.game}/{old_unit["name"]}"
        unit["target_path"] = str(project.rebase_relative_path(old_unit["target_path"]))
        if "base_path" in old_unit:
            unit["base_path"] = str(project.rebase_relative_path(old_unit["base_path"]))
        
        if "scratch" in old_unit:
            scratch = {}
            old_scratch: dict = old_unit["scratch"]
            scratch["platform"] = old_scratch["platform"]
            scratch["compiler"] = old_scratch["compiler"]
            scratch["c_flags"] = old_scratch["c_flags"]
            scratch["ctx_path"] = str(project.rebase_relative_path(old_scratch["ctx_path"]))
            scratch["build_ctx"] = old_scratch["build_ctx"]
            unit["scratch"] = scratch

        metadata = {}
        old_metadata: dict = old_unit["metadata"]
        metadata["complete"] = old_metadata["complete"]
        metadata["reverse_fn_order"] = old_metadata["reverse_fn_order"]
        if "source_path" in old_metadata:
            metadata["source_path"] = str(project.rebase_relative_path(old_metadata["source_path"]))
        metadata["auto_generated"] = old_metadata["auto_generated"]
        unit["metadata"] = metadata

        units.append(unit)

result["units"] = units

with output_path.open("w") as f:
    f.write(json.dumps(result, indent=2))
