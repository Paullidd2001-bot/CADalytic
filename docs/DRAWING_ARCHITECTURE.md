# Drawing Architecture

Drawings are a first-class consumer of model geometry and references, positioned before CAM in the roadmap.

The initial model consists of `DrawingDocument`, `DrawingSheet`, `DrawingView`, model-backed base/projected/section/detail views, annotations, dimensions, title blocks, and parts lists. A view stores a source document identity, configuration when supported, projection parameters, display options, and reference diagnostics.

Drawing updates occur after a successful model rebuild. Broken or ambiguous associations remain visible as diagnostics and never silently move a dimension to unrelated geometry. PDF output is the first export target; DXF remains subject to dependency and licensing review.

The drawing module must depend on application geometry and document/reference interfaces, not directly on UI widgets. Headless tests cover projection, hidden-line decisions, association survival, broken references, and update after model change.
